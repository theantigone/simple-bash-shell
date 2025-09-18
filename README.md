# Simple Bash Shell

- [Tech Stack](#tech-stack)
- [Features](#features)
- [Getting Started](#getting-started)
- [Testing](#testing)

A simple shell implemented in C to demonstrate process management concepts in Linux

## Tech Stack
- C
- Make

## Features

Includes these built-in Linux commands:
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

## Getting Started

### Prerequisites
- `gcc`
- `make`

### Installation & Usage
1. Clone the repository:
```bash
git clone https://github.com/theantigone/simple-bash-shell.git
cd simple-bash-shell
```
2. Compile the program:
```bash
make
```
3. Run the shell:
```bash
./bin/my_shell
```

> [!NOTE]
> To delete the executable and object files, run:
> ```bash
> make clean
> ```

## Testing
To run the provided test suite:
```bash
cd tests
./master.sh ../src/my_shell.c
```
