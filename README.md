# Simple Bash Shell

## Goal

To develop a simple shell akin to Bash, and to deepen my understanding of process management in Linux.

## Description

Includes these Linux built-in commands:
- `fork`
- `exec`
- `exit`
- `wait`
- `ls`
- `cat`
- `echo`
- `sleep`
- `cd`

If a command is followed by `&`, the command is executed in the background (e.g., `sleep 10 &`) and returns the prompt to the user for the next input.

Multiple commands separated by `&&` are executed one after the other sequentially in the foreground (e.g., `sleep 10 && echo hi`).

Multiple commands separated by `&&&` are executed in parallel in the foreground: the shell executes all of the commands simultaneously (e.g., `ls &&& echo hi &&& sleep 10`)

A command not followed by any of the special characters above are executed in the foreground.

The `Ctrl + C` command terminates all foreground processes: when executing multiple commands in serial mode, the shell terminates the current command, ignores all subsequent commands in the series, and returns the prompt back to the user.
