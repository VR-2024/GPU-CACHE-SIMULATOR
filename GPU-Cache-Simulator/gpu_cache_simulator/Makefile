CC = gcc
CFLAGS = -Wall -Wextra -O3 -std=c99 -Iinclude

TARGET = gpu_cache_simulator

# Collect all source files from the src directory
C_FILES = src/main.c src/hash_table.c src/queue.c src/deque.c src/priority_queue.c src/cache_layer.c src/gpu_memory_system.c src/utils.c

# Generate object file names
OBJECTS = $(C_FILES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) -lm

# Rule to compile .c files from src/ into .o files (in the same src/ folder)
src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)
	rm -f data/*.txt.out

test: $(TARGET)
	./$(TARGET) data/memory_trace.txt
