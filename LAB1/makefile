# Compiler and flags
CC = gcc # Compiler
CFLAGS = -Wall -O0 -fopenmp # Compiler flags
CFLAGS += -DDEBUG -DSEQUENCE # Additional flags for debugging and sequence mode
# Target executable
TARGET = sequence_convolution

# Source files
SRCS = read_matrix.c sequence_convolution.c 

# Object files
OBJS = $(SRCS:.c=.o) # Convert .c files to .o files

# Default target: build everything
all: $(TARGET)
	@echo "build complete"

# Link object file into final executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^
	@echo "Linking complete"

# Rule to compile .c files to .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled $< to $@"

# Clean up build files
clean:
	rm -f $(OBJS) $(TARGET)
	@echo "Cleaned up build files"
