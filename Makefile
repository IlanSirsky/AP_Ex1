CC = gcc
CFLAGS = -Wall

# Define the source files and object files
SRC_FILES = myshell.c types.c handlers.c
OBJ_FILES = $(SRC_FILES:.c=.o)

# Define the target executable
TARGET = myshell

# Default target
all: $(TARGET)

# Compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link object files into the target executable
$(TARGET): $(OBJ_FILES)
	$(CC) $(CFLAGS) $^ -o $@

# Dependencies
myshell.o: myshell.c handlers.h
types.o: types.c types.h
handlers.o: handlers.c handlers.h types.h

# Clean
clean:
	rm -f $(OBJ_FILES) $(TARGET)