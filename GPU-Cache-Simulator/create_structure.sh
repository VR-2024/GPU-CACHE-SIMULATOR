#!/bin/bash

PROJECT_DIR="gpu_cache_simulator"

# 1. Create the root directory and navigate into it
mkdir -p $PROJECT_DIR
cd $PROJECT_DIR

# 2. Create primary subdirectories: src, include, data, docs, bin
mkdir -p src include data docs bin

# 3. Create source files (.c) in src/
touch src/main.c
touch src/hash_table.c
touch src/queue.c
touch src/deque.c
touch src/priority_queue.c
touch src/cache_layer.c
touch src/gpu_memory_system.c
touch src/utils.c

# 4. Create header files (.h) in include/
touch include/hash_table.h
touch include/queue.h
touch include/deque.h
touch include/priority_queue.h
touch include/cache_layer.h
touch include/gpu_memory_system.h
touch include/utils.h

# 5. Create root-level files (Makefile and .gitignore)
touch Makefile
touch .gitignore

# 6. Create placeholder files in data/ and docs/
# Create an initial sample trace file in data/
echo "R 0x1000000 4 0 0" > data/memory_trace.txt
# Create placeholder documentation files in docs/
touch docs/README.md
touch docs/QUICKSTART.md
touch docs/PROJECT_INDEX.md
touch docs/GPU_Cache_Simulator_Complete_Documentation.pdf

echo "Directory structure created in ./$PROJECT_DIR/"
