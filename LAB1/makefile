# Compiler and flags
CC = gcc # Compiler
CFLAGS = -Wall -O0 -fopenmp -g  # Compiler flags
# CFLAGS += -DDEBUG  # Additional flags for debugging and sequence mode
CFLAGS += -DPARALLEL_V1  # Additional flags for sequence mode
CFLAGS += -DWRITEOUT # Additional flags for writing output matrix to file
# Target executable
TARGET = parallel_convolution_v1 # Name of the output executable

# Source files
SRCS = read_matrix.c convolution.c 

# Object files
OBJS = $(SRCS:.c=.o) # Convert .c files to .o files

# Default target: build everything
all: $(TARGET)
	CFLAGS += -DSEQUENCE
	@echo "build complete"
paral_v1: CFLAGS += -DPARALLEL_V1
paral_v1: $(TARGET)
	@echo "build complete"
paral_v2: CFLAGS += -DPARALLEL_V2
paral_v2: $(TARGET)
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
