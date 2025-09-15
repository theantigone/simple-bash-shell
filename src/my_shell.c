#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h> // For signal handling

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

// Structure to hold information about a background process
typedef struct {
	pid_t pid;
	int active; // 1 if active, 0 if finished
} BackgroundProcess;

BackgroundProcess background_jobs[MAX_NUM_TOKENS]; // Array to store background jobs
int num_background_jobs = 0; // Keep track of the number of active background jobs

// --- Signal Handler for Ctrl+C (SIGINT) ---
void sigint_handler(int sig) {
	// Do nothing here.  The mere presence of this handler
	// prevents the shell from exiting.  The signal will
	// still be delivered to the foreground process group.
}
int count;

/* Splits the string by space and returns the array of tokens
*/
char **tokenize(char *line) {
	char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
	char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
	int i, tokenIndex = 0, tokenNo = 0;

	for(i =0; i < strlen(line); i++){

		char readChar = line[i];

		if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
			token[tokenIndex] = '\0';
			if (tokenIndex != 0){
				tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
				strcpy(tokens[tokenNo++], token);
				tokenIndex = 0;
			}
		} else {
			token[tokenIndex++] = readChar;
		}
	}

	free(token);
	count = tokenNo; // debugging purposes
	tokens[tokenNo] = NULL ;
	return tokens;
}

// Function to handle the 'cd' command
int handle_cd(char **tokens) {
	if (tokens[1] == NULL) {
		fprintf(stderr, "Shell: Incorrect command\n");
//printf("%s %s %s %s %s\n",tokens[0],tokens[1],tokens[2],tokens[3],tokens[4]);
		return -1;
	} else {
		if (chdir(tokens[1]) != 0) {
			fprintf(stderr, "Shell: Incorrect command\n");
//printf("%s %s %s %s %s\n",tokens[0],tokens[1],tokens[2],tokens[3],tokens[4]);
			return -1;
		}
	}
//printf("%s %s %s %s %s\n",tokens[0],tokens[1],tokens[2],tokens[3],tokens[4]);
	return 0;
}

// Function to reap finished background processes
void reap_background_processes() {
	int status;
	pid_t pid;
	// Loop through our list of background jobs
	for (int i = 0; i < num_background_jobs; i++) {
		if (background_jobs[i].active) {
			// Check if the process has finished *without* blocking
			pid = waitpid(background_jobs[i].pid, &status, WNOHANG);

			if (pid == background_jobs[i].pid) {
				// The process has finished
				printf("Shell: Background process finished\n");
				background_jobs[i].active = 0; // Mark it as inactive
			} else if (pid == -1 && errno != ECHILD) {
				//ECHILD means no children, which is fine.
				//Other errors should be reported.
				perror("waitpid failed");
			}
		}
	}
}

// Function to execute a single command (either foreground or background)
pid_t execute_command(char **tokens, int is_background) {
	pid_t pid = fork();

	if (pid == -1) {
		perror("fork failed");
		return -1; // Return -1 on fork failure
	} else if (pid == 0) {
		// Child process:

		// --- Set Process Group ID (for Ctrl+C handling) ---
		if (is_background) {
			setpgid(0, 0); // Put background processes in their own group
		} else {
			setpgid(0, getpgrp()); // Put foreground processes in the shell's group
		}

		if (execvp(tokens[0], tokens) == -1) {
			fprintf(stderr, "Shell: Incorrect command\n");
			exit(EXIT_FAILURE); // Exit the CHILD on failure!
		}
	} else {
		// Parent process:
		if (is_background) {
			// Add the process to our list of background jobs
			background_jobs[num_background_jobs].pid = pid;
			background_jobs[num_background_jobs].active = 1;
			num_background_jobs++;
			printf("Shell: Background process started with PID %d\n", pid);
		}
	}
//printf("%s %s %s %s %s\n",tokens[0],tokens[1],tokens[2],tokens[3],tokens[4]);
//	if (strcmp(tokens[0], "cd") == 0) {
//		tokens[0] == NULL;
//	}
	return pid; // Return the child's PID
}


// Function to handle parallel execution of commands
void handle_parallel_execution(char **tokens) {
	pid_t pids[MAX_NUM_TOKENS];
	int num_pids = 0;

	// Iterate through the tokens, executing each command
	int i = 0;
	while (tokens[i] != NULL) {
		int j = i;
		while (tokens[j] != NULL && strcmp(tokens[j], "&&&") != 0) {
			j++;
		}
		//Make sure command section is null terminated
		char *separator = tokens[j];
		tokens[j] = NULL;

		//Execute command and get pid
		pid_t pid = execute_command(tokens + i, 0); // Foreground execution within parallel
		if(pid != -1){
			pids[num_pids++] = pid;
		}

		if (separator == NULL) break;
		i = j + 1;
	}

	// Wait for all child processes to finish
	for (int k = 0; k < num_pids; k++) {
		int status;
		waitpid(pids[k], &status, 0); //wait for each process.
	}
}

// Function to handle the 'exit' command
void handle_exit() {
	// Kill all background processes
	for (int i = 0; i < num_background_jobs; i++) {
		if (background_jobs[i].active) {
			kill(background_jobs[i].pid, SIGTERM); // Send SIGTERM
		}
	}

	// Wait for all background processes to terminate
	reap_background_processes(); // Reap any that finished immediately

	// printf("Exiting shell...\n");
	exit(0); // Exit the shell
}

int main(int argc, char *argv[]) {
	char line[MAX_INPUT_SIZE];
	char **tokens;
	int i;

	FILE *fp;
	if (argc == 2) {
		fp = fopen(argv[1], "r");
		if (fp < 0) {
			printf("File doesn't exist.\n");
			return -1;
		}
	}
	//initialize background jobs array
	for(i = 0; i<MAX_NUM_TOKENS; i++){
		background_jobs[i].active = 0;
	}

	// --- Set up the signal handler for Ctrl+C ---
	struct sigaction sa;
	sa.sa_handler = sigint_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART; // Restart system calls if interrupted

	if (sigaction(SIGINT, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	while (1) {
		// Reap background processes before getting new input
		reap_background_processes();
		/* BEGIN: TAKING INPUT */
		bzero(line, sizeof(line));
		if (argc == 2) {  // batch mode
			if (fgets(line, sizeof(line), fp) == NULL) {  // file reading finished
				break;
			}
			line[strlen(line) - 1] = '\0';
		} else {  // interactive mode
			printf("$ ");
			scanf("%[^\n]", line);
			getchar();
		}
		/* END: TAKING INPUT */

		line[strlen(line)] = '\n'; //terminate with new line
		tokens = tokenize(line);

		if (tokens[0] == NULL) { // Empty command
								 // Free tokens array
			for (int k = 0; tokens[k] != NULL; k++) {
				free(tokens[k]);
			}
			free(tokens);
			continue;
		}

		// --- Check for the 'exit' command BEFORE forking ---
		if (strcmp(tokens[0], "exit") == 0) {
			handle_exit(); // This will exit the shell
		}


		if (strcmp(tokens[0], "cd") == 0) {
			handle_cd(tokens);
			// Free tokens array
			for (int k = 0; tokens[k] != NULL; k++) {
				free(tokens[k]);
			}
			free(tokens);
			continue;  // Go back to the beginning of the loop
		}
		// --- Logic for handling &, &&, and &&& ---
		i = 0;
		while (tokens[i] != NULL) {
//printf("%s %s %s %s %s",tokens[0],tokens[1],tokens[2],tokens[3],tokens[4]);
//printf("BEGIN\n");
//printf("%s, token\n",tokens[i]);
//printf("%d, index number\n",i);
//printf("%d, number of tokens in array\n",count);
			int j = i;
			while (tokens[j] != NULL && strcmp(tokens[j], "&") != 0 &&
					strcmp(tokens[j], "&&") != 0 && strcmp(tokens[j], "&&&") != 0) {
				j++;
//printf("%d, index after &, &&, or NULL symbol\n",j);
//printf("%s %s %s %s %s",tokens[0],tokens[1],tokens[2],tokens[3],tokens[4]);
			}

			char *separator = tokens[j];
			tokens[j] = NULL; // Null-terminate the current command

			if (separator == NULL || strcmp(separator, "&&") == 0) {
				// Foreground or Serial Execution
				pid_t pid = execute_command(tokens + i, 0); // Execute in foreground
				tokens[j] = separator;
				if (pid != -1) { // Only wait if fork succeeded
					int status;
					waitpid(pid, &status, 0); // Wait for the specific child
				}
//printf("END\n");
			} else if (strcmp(separator, "&") == 0) {
				// Background Execution
				execute_command(tokens + i, 1); // Execute in background (don't wait)
			} else if (strcmp(separator, "&&&") == 0) {
				// Parallel Execution
				handle_parallel_execution(tokens+i);
				tokens[j] = separator;
			}

			i = j + 1; // Move past the separator (or to NULL)
		}

		for (int k = 0; tokens[k] != NULL; k++) {
			free(tokens[k]);
			tokens[k] = NULL; // Set the pointer to NULL after freeing
		}
		free(tokens);
		tokens = NULL; // Set the main tokens pointer to NULL
	}
	// Wait for any remaining background processes before exiting
	reap_background_processes();

	if (argc == 2) {
		fclose(fp);
	}
	return 0;
}
