#ifndef CACHE_LAYER_H
#define CACHE_LAYER_H

#include "utils.h"
#include "hash_table.h"
#include "queue.h"
#include "deque.h"
#include "priority_queue.h"

#define MAX_CACHE_SETS 16384 // Cap for array size

typedef enum {
    REPLACEMENT_LRU,
    REPLACEMENT_FIFO,
    REPLACEMENT_LFU,
    REPLACEMENT_RANDOM
} replacement_policy_t;

// --- Cache Block Structure ---
typedef struct {
    uint64_t tag;
    bool valid;
    bool dirty; // Indicates data was modified (needs writeback)
    uint32_t access_time; // For LRU counter
    uint32_t access_count; // For LFU policy
    uint8_t data[CACHE_LINE_SIZE]; // Simulated data storage
} cache_block_t;

// --- Cache Set Structure ---
typedef struct {
    cache_block_t* blocks;
    uint32_t associativity;
    uint32_t lru_counter; // Used to track time for blocks in the set
    
    // Policy-specific data structures
    queue_t* fifo_queue;
    deque_t* lru_deque;
    priority_queue_t* lfu_heap;
} cache_set_t;

// --- Cache Layer (L1, L2, Shared Memory) Structure ---
typedef struct cache_layer_t {
    char name[32];
    uint32_t size;
    uint32_t block_size;
    uint32_t associativity;
    replacement_policy_t policy;
    uint32_t latency;
    
    uint32_t num_sets;
    cache_set_t* sets;
    hash_table_t* tag_table; // Maps address tag to block index (optional optimization)
    
    uint64_t hits;
    uint64_t misses;
    uint64_t evictions;
    
    struct cache_layer_t* next_level; // Pointer to the next cache level or global memory
} cache_layer_t;

// --- Functions ---
cache_layer_t* cache_layer_create(
    const char* name,
    uint32_t size,
    uint32_t block_size,
    uint32_t associativity,
    replacement_policy_t policy,
    uint32_t latency
);

bool cache_access(cache_layer_t* cache, memory_access_t* access);
double get_hit_rate(cache_layer_t* cache);
double get_miss_rate(cache_layer_t* cache);
void print_cache_stats(cache_layer_t* cache);
void cache_layer_free(cache_layer_t* cache);

#endif // CACHE_LAYER_H
