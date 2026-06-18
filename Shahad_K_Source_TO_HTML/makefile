# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g
# -g adds debugging symbols so you can use gdb/valgrind

# The final executable name
TARGET = s2html
TEST_TARGET = test_runner

# Object files needed for the main program
OBJ = s2html_main.o s2html_event.o s2html_conv.o

# Object files needed for the test program (we don't include main.o here)
TEST_OBJ = s2html_event.o s2html_conv.o test_main.o

# Default rule: build the main program
all: $(TARGET)

# Link the object files to create the executable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# Build the unit test executable
test: $(TEST_OBJ)
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(TEST_OBJ)
	./$(TEST_TARGET)

# Generic rule to compile .c files into .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f *.o $(TARGET) $(TEST_TARGET)