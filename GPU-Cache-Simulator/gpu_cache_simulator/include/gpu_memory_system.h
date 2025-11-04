#ifndef GPU_MEMORY_SYSTEM_H
#define GPU_MEMORY_SYSTEM_H

#include "cache_layer.h"
#include "utils.h"

// --- Configuration Parameters ---
#define NUM_REGISTERS_PER_THREAD 256 // Each register is 4 bytes
#define SHARED_MEMORY_SIZE (64 * 1024) // 64KB (Direct-mapped cache logic used)
#define L1_CACHE_SIZE (64 * 1024)      // 64KB
#define L1_ASSOCIATIVITY 4
#define L2_CACHE_SIZE (4 * 1024 * 1024)  // 4MB
#define L2_ASSOCIATIVITY 16
#define GLOBAL_MEMORY_SIZE (1024ULL * 1024 * 1024) // 1GB

// --- GPU System Structure ---
typedef struct {
    // Registers (Fastest level, direct access)
    uint32_t* register_files[MAX_THREADS]; // Simulated per-thread registers
    uint64_t register_hits;

    // Cache Hierarchy
    cache_layer_t* shared_memory; // L1 Shared Memory (used as Direct-Mapped cache)
    cache_layer_t* l1_cache;      // L1 Cache (Per-SM)
    cache_layer_t* l2_cache;      // L2 Cache (Global)

    // Global Memory (Main storage, slowest level)
    uint8_t* global_memory; // 1GB of simulated global memory
    uint64_t global_memory_size;
    uint64_t global_memory_accesses;

    // Statistics
    uint64_t total_accesses;
    uint32_t current_cycle;
} gpu_memory_system_t;

// --- Functions ---
gpu_memory_system_t* create_gpu_memory_system(void);
uint32_t gpu_memory_access(gpu_memory_system_t* system, memory_access_t* access);
void print_gpu_system_stats(gpu_memory_system_t* system);
void free_gpu_memory_system(gpu_memory_system_t* system);

#endif // GPU_MEMORY_SYSTEM_H
