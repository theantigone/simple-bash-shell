# Compiler
CC = gcc

# Compiler flags
#   -Wall: Enable all warnings
#   -g:    Enable debugging information (useful for gdb)
#   -O2:   Optimize for performance (optional, but good for final builds)
CFLAGS = -Wall -g -O2

# Name of the executable
TARGET = my_shell

# Source files
SRC = my_shell.c

# Object files (automatically generated from SRC)
OBJ = $(SRC:.c=.o)

# Default target: build the executable
all: $(TARGET)

# Rule to build the executable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# Rule to compile a .c file into a .o file
#   $@:  The target file (the .o file)
#   $<:  The first prerequisite (the .c file)
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Clean up: remove object files and the executable
clean:
	rm -f $(OBJ) $(TARGET)

# Phony targets (targets that are not actual files)
.PHONY: all clean
