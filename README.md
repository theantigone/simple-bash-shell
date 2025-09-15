# Simple Bash Shell

- [Goal](#goal)
- [Description](#description)
- [Compiling](#compiling)

## Goal

To develop a simple shell akin to Bash, and to deepen my understanding of process management in Linux.

## Description

Includes these built-in Linux commands:
- `fork`
- `exec`
- `exit`
- `wait`
- `ls`
- `cat`
- `echo`
- `sleep`
- `cd`

If a command is followed by `&`, the command is executed in the background and returns the prompt to the user for the next input.

```bash
# example of "&"

sleep 10 &
```

Multiple commands separated by `&&` are executed one after the other sequentially in the foreground.

```bash
# example of "&&"

sleep 10 && echo hi
```

Multiple commands separated by `&&&` are executed in parallel in the foreground: the shell executes all of the commands simultaneously.

```bash
# example of "&&&"

sleep 10 &&& echo hi &&& ls
```

A command not followed by any of the special characters above are executed in the foreground.

The `Ctrl + C` command terminates all foreground processes: when executing multiple commands in serial mode, the shell terminates the current command, ignores all subsequent commands in the series, and returns the prompt back to the user.

## Compiling

1. Navigate to the `src` directory
2. Run:
```bash
make
```
to turn the source code into an executable
3. Run:
```bash
./my_shell
```
> [!NOTE]
> To delete the executable and object files, run:
> ```bash
> make clean
