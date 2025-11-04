#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_THREADS 1024
#define MAX_BLOCKS 32
#define CACHE_LINE_SIZE 128

// --- Data Types ---

typedef enum {
    ACCESS_READ,
    ACCESS_WRITE
} access_type_t;

typedef struct {
    char operation;  // 'R' or 'W'
    uint64_t address;
    uint32_t size;
    uint32_t thread_id;
    uint32_t block_id;
} memory_trace_t;

typedef struct {
    uint64_t address;
    access_type_t type;
    uint32_t thread_id;
    uint32_t block_id;
} memory_access_t;

// --- Functions ---
int load_memory_trace(const char* filename, memory_trace_t** traces, uint32_t* count);
void free_memory_trace(memory_trace_t* traces);
void print_trace_summary(memory_trace_t* traces, uint32_t count);

#endif // UTILS_H
