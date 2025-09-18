# Compiler
CC = gcc

# Compiler flags
#   -Wall: Enable all warnings
#   -g:    Enable debugging information (useful for gdb)
#   -O2:   Optimize for performance (optional, but good for final builds)
CFLAGS = -Wall -g -O2

# Source directory
SRCDIR = src

# Object directory
OBJDIR = obj

# Binary directory
BINDIR = bin

# Find all .c files in the source directory
SRC = $(wildcard $(SRCDIR)/*.c)

# Create object file names from source file names
OBJ = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRC))

# Name of the final executable
TARGET = $(BINDIR)/my_shell

# Default target: build the executable
all: $(TARGET)

# Rule to build the executable
$(TARGET): $(OBJ)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^

# Rule to compile a .c file into a .o file
#   $@:  The target file (the .o file)
#   $<:  The first prerequisite (the .c file)
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

# Clean up: remove object files and the executable
clean:
	rm -rf $(OBJDIR) $(BINDIR)

# Phony targets (targets that are not actual files)
.PHONY: all clean
