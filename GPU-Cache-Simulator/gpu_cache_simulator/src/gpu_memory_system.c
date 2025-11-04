#include "gpu_memory_system.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

gpu_memory_system_t* create_gpu_memory_system(void) {
    gpu_memory_system_t* system = (gpu_memory_system_t*)malloc(sizeof(gpu_memory_system_t));
    if (!system) return NULL;

    // Initialize Registers
    for (int i = 0; i < MAX_THREADS; i++) {
        // Simulate 256 registers, 4 bytes each
        system->register_files[i] = (uint32_t*)calloc(NUM_REGISTERS_PER_THREAD, sizeof(uint32_t));
    }

    // Create Cache Layers
    system->shared_memory = cache_layer_create("Shared Memory (L1 Scratchpad)", SHARED_MEMORY_SIZE,
        CACHE_LINE_SIZE, 1, REPLACEMENT_RANDOM, 20); // Direct Mapped/Random
        
    system->l1_cache = cache_layer_create("L1 Cache (Per-SM)", L1_CACHE_SIZE,
        CACHE_LINE_SIZE, L1_ASSOCIATIVITY, REPLACEMENT_LRU, 30);

    system->l2_cache = cache_layer_create("L2 Cache (Global)", L2_CACHE_SIZE,
        CACHE_LINE_SIZE, L2_ASSOCIATIVITY, REPLACEMENT_LRU, 200);

    // Link the Hierarchy
    // Shared Memory is not strictly part of the cache hierarchy, but for simulation, we link it as a layer before L2
    system->shared_memory->next_level = system->l2_cache; 
    system->l1_cache->next_level = system->l2_cache;

    // Initialize Global Memory
    system->global_memory_size = GLOBAL_MEMORY_SIZE;
    system->global_memory = (uint8_t*)calloc(GLOBAL_MEMORY_SIZE, sizeof(uint8_t));

    // Initialize Statistics
    system->total_accesses = system->register_hits = system->global_memory_accesses = 0;
    system->current_cycle = 0;

    return system;
}

// Simplified check: is the address in the register address space for this thread?
bool is_register_address(uint64_t address, uint32_t thread_id) {
    uint64_t thread_register_base = (uint64_t)thread_id * NUM_REGISTERS_PER_THREAD * 4;
    return address < thread_register_base + NUM_REGISTERS_PER_THREAD * 4;
}

// Simplified check: is the address in the shared memory address space for this block?
bool is_shared_memory_address(uint64_t address, uint32_t block_id) {
    // In a real GPU, shared memory addresses are virtual and small.
    // For this simulation, we check if the address is very small and non-zero.
    return address > 0 && address < SHARED_MEMORY_SIZE * MAX_BLOCKS; 
}

uint32_t gpu_memory_access(gpu_memory_system_t* system, memory_access_t* access) {
    if (!system || !access) return 0;

    system->total_accesses++;
    uint32_t total_latency = 0;

    // 1. Check Registers
    if (is_register_address(access->address, access->thread_id)) {
        system->register_hits++;
        return 1; // 1 cycle
    }

    // 2. Check Shared Memory
    if (is_shared_memory_address(access->address, access->block_id)) {
        bool hit = cache_access(system->shared_memory, access);
        total_latency += system->shared_memory->latency;
        
        // If it's a shared memory access, we treat the remaining hierarchy as separate.
        // We only proceed to L1/L2 if it's NOT a dedicated shared memory access.
        if (hit) return total_latency;
    }

    // 3. Check L1 Cache
    // Snapshot L2 stats so we can detect what happened during the L1 access (cache_access calls next level on miss).
    uint64_t l2_hits_before = system->l2_cache ? system->l2_cache->hits : 0;
    uint64_t l2_misses_before = system->l2_cache ? system->l2_cache->misses : 0;

    bool l1_hit = cache_access(system->l1_cache, access);
    total_latency += system->l1_cache->latency;

    if (l1_hit) return total_latency;

    // L1 miss: determine whether L2 was hit or missed by comparing deltas
    if (system->l2_cache) {
        uint64_t l2_hits_after = system->l2_cache->hits;
        uint64_t l2_misses_after = system->l2_cache->misses;

        uint64_t l2_hits_delta = (l2_hits_after >= l2_hits_before) ? (l2_hits_after - l2_hits_before) : 0;
        uint64_t l2_misses_delta = (l2_misses_after >= l2_misses_before) ? (l2_misses_after - l2_misses_before) : 0;

        // If L2 was accessed and produced a hit
        if (l2_hits_delta > 0) {
            total_latency += system->l2_cache->latency;
        } else if (l2_misses_delta > 0) {
            // L2 miss -> went to global memory
            total_latency += system->l2_cache->latency;
            system->global_memory_accesses++;
            total_latency += 400; // Global memory latency
        } else {
            // Fallback: if L2 stats didn't change, conservatively add L2 latency
            total_latency += system->l2_cache->latency;
        }
    } else {
        // No L2 configured: go straight to global memory
        system->global_memory_accesses++;
        total_latency += 400;
    }

    // Return cumulative latency for this access
    return total_latency;
}

void print_gpu_system_stats(gpu_memory_system_t* system) {
    printf("\n\nGPU Cache & Memory Hierarchy Statistics\n");
    printf("=======================================\n");
    printf("Total Memory Accesses: %lu\n", system->total_accesses);
    printf("Total Simulation Cycles: %u\n", system->current_cycle);
    printf("Register Hits: %lu\n", system->register_hits);
    printf("Global Memory Accesses (L2 Misses): %lu\n\n", system->global_memory_accesses);

    print_cache_stats(system->shared_memory);
    print_cache_stats(system->l1_cache);
    print_cache_stats(system->l2_cache);

    if (system->total_accesses > 0) {
        double avg_latency = (double)system->current_cycle / system->total_accesses;
        printf("Average Memory Access Time: %.2f cycles\n", avg_latency);
    }
}

void free_gpu_memory_system(gpu_memory_system_t* system) {
    if (!system) return;

    for (int i = 0; i < MAX_THREADS; i++) free(system->register_files[i]);
    
    cache_layer_free(system->shared_memory);
    cache_layer_free(system->l1_cache);
    cache_layer_free(system->l2_cache);
    
    if (system->global_memory) free(system->global_memory);
    free(system);
}
