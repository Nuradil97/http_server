# Compiler to use
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra

# Linker flags (for Windows and Winsock)
LDFLAGS = -lws2_32

# Target executable
TARGET = server_app

# Source files
SRCS = main.c server.c

# Object files
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET)

# Rule to build the target executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Rule to build the object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up the build files
clean:
	rm -f $(OBJS) $(TARGET)