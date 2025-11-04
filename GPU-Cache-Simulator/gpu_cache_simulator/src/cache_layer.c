#include "cache_layer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

cache_layer_t* cache_layer_create(
    const char* name,
    uint32_t size,
    uint32_t block_size,
    uint32_t associativity,
    replacement_policy_t policy,
    uint32_t latency
) {
    cache_layer_t* cache = (cache_layer_t*)malloc(sizeof(cache_layer_t));
    if (!cache) return NULL;
    
    strncpy(cache->name, name, sizeof(cache->name) - 1);
    cache->size = size;
    cache->block_size = block_size;
    cache->associativity = associativity;
    cache->policy = policy;
    cache->latency = latency;
    
    cache->num_sets = size / (block_size * associativity);
    if (cache->num_sets == 0) cache->num_sets = 1; // Handle fully-associative case (1 set)
    
    cache->hits = 0;
    cache->misses = 0;
    cache->evictions = 0;
    cache->next_level = NULL;
    
    cache->sets = (cache_set_t*)calloc(cache->num_sets, sizeof(cache_set_t));
    if (!cache->sets) {
        free(cache);
        return NULL;
    }
    
    for (uint32_t i = 0; i < cache->num_sets; i++) {
        cache->sets[i].blocks = (cache_block_t*)calloc(associativity, sizeof(cache_block_t));
        cache->sets[i].associativity = associativity;
        cache->sets[i].lru_counter = 0;
        
        if (policy == REPLACEMENT_FIFO) {
            cache->sets[i].fifo_queue = queue_create();
        } else if (policy == REPLACEMENT_LRU) {
            // Note: LRU for LRU policy is done via block->access_time in this simplified model.
            // Deque is used for advanced LRU tracking if hash table were used to map tag->block index.
            cache->sets[i].lru_deque = deque_create();
        } else if (policy == REPLACEMENT_LFU) {
            cache->sets[i].lfu_heap = pq_create(associativity);
        }
    }
    
    // Hash table is not used in this iteration of set-associative logic, 
    // as direct-mapped array access is faster when set index is known.
    cache->tag_table = NULL; 
    
    cache->next_level = NULL;
    return cache;
}

// Find an invalid block, or the victim based on policy
uint32_t find_victim_block(cache_layer_t* cache, uint32_t set_idx) {
    cache_set_t* set = &cache->sets[set_idx];
    
    // First, check for an empty (invalid) block
    for (uint32_t i = 0; i < set->associativity; i++) {
        if (!set->blocks[i].valid) return i;
    }
    
    // No empty block, must evict based on policy
    switch (cache->policy) {
        case REPLACEMENT_LRU: {
            uint32_t victim = 0;
            uint32_t min_time = set->blocks[0].access_time;
            
            for (uint32_t i = 1; i < set->associativity; i++) {
                if (set->blocks[i].access_time < min_time) {
                    min_time = set->blocks[i].access_time;
                    victim = i;
                }
            }
            return victim;
        }
        
        case REPLACEMENT_FIFO: {
            // FIFO in this implementation relies on finding the victim with the oldest insertion time. 
            // Since we don't track insertion time, we use the first valid block (simplification for array model)
            // Or use the queue for a more accurate model (index is stored in the queue)
            if (set->fifo_queue) return queue_dequeue(set->fifo_queue);
            return 0;
        }
        
        case REPLACEMENT_RANDOM: {
            return rand() % set->associativity;
        }
        
        case REPLACEMENT_LFU: {
            // This implementation uses the block with the lowest access count
            uint32_t victim = 0;
            uint32_t min_count = set->blocks[0].access_count;
            
            for (uint32_t i = 1; i < set->associativity; i++) {
                if (set->blocks[i].access_count < min_count) {
                    min_count = set->blocks[i].access_count;
                    victim = i;
                }
            }
            return victim;
        }
        
        default:
            return 0;
    }
}

bool cache_access(cache_layer_t* cache, memory_access_t* access) {
    if (!cache) return false;
    
    uint64_t block_addr = access->address / cache->block_size;
    uint32_t set_idx = block_addr % cache->num_sets;
    uint64_t tag = block_addr / cache->num_sets;

    cache_set_t* set = &cache->sets[set_idx];

    // 1. Check for a Hit
    uint32_t hit_idx = cache->associativity; // Sentinel value
    for (uint32_t i = 0; i < set->associativity; i++) {
        cache_block_t* block = &set->blocks[i];
        if (block->valid && block->tag == tag) {
            hit_idx = i;
            break;
        }
    }

    if (hit_idx < cache->associativity) {
        // HIT: Update statistics and metadata
        cache->hits++;
        cache_block_t* block = &set->blocks[hit_idx];
        
        // Update access time/count for LRU/LFU
        block->access_time = set->lru_counter++;
        block->access_count++;
        
        if (access->type == ACCESS_WRITE) block->dirty = true;
        
        return true;
    }

    // 2. Miss: Go to next level
    cache->misses++;

    // Calculate latency for the fetch from the next level
    uint32_t next_level_latency = 0;
    if (cache->next_level) {
        // Recursive call to access the next level
        cache_access(cache->next_level, access);
        // The time taken is primarily the latency of the next level plus any additional access time.
        // In this simple model, we assume the cost is the next level's latency.
        next_level_latency = cache->next_level->latency;
    } 
    // In a real simulator, next_level_latency would be returned by the recursive call. 
    // For this demonstration, we focus on cache state updates.

    // 3. Eviction/Insertion (if data is not available from lower levels, it's inserted here)
    
    uint32_t victim_idx = find_victim_block(cache, set_idx);
    cache_block_t* victim = &set->blocks[victim_idx];

    // Writeback check
    if (victim->valid && victim->dirty && cache->next_level) {
        // Writeback the dirty block to the next level
        cache->evictions++;
        
        // This is a simplified writeback access to the next level (not tracked for stats)
        // A real simulator would perform a separate access here.
    }

    // Install New Block
    victim->valid = true;
    victim->tag = tag;
    victim->dirty = (access->type == ACCESS_WRITE);
    
    // Reset metadata for the new block
    victim->access_time = set->lru_counter++;
    victim->access_count = 1;
    
    // Update FIFO/LRU tracking structures if used for selection
    if (cache->policy == REPLACEMENT_FIFO) {
        // Enqueue the new block index
        queue_enqueue(set->fifo_queue, victim_idx);
    } 
    
    return false;
}

double get_hit_rate(cache_layer_t* cache) {
    uint64_t total = cache->hits + cache->misses;
    return total == 0 ? 0.0 : (double)cache->hits / total * 100.0;
}

double get_miss_rate(cache_layer_t* cache) {
    uint64_t total = cache->hits + cache->misses;
    return total == 0 ? 0.0 : (double)cache->misses / total * 100.0;
}

void print_cache_stats(cache_layer_t* cache) {
    printf("%s Statistics:\n", cache->name);
    printf("  Size: %u KB, Associativity: %u, Sets: %u\n",
        cache->size / 1024, cache->associativity, cache->num_sets);
    printf("  Hits: %lu, Misses: %lu\n", cache->hits, cache->misses);
    printf("  Hit Rate:  %.2f%%\n", get_hit_rate(cache));
    printf("  Miss Rate: %.2f%%\n", get_miss_rate(cache));
    printf("  Evictions: %lu\n", cache->evictions);
    printf("  Latency: %u cycles\n\n", cache->latency);
}

void cache_layer_free(cache_layer_t* cache) {
    if (!cache) return;
    
    for (uint32_t i = 0; i < cache->num_sets; i++) {
        free(cache->sets[i].blocks);
        if (cache->sets[i].fifo_queue) queue_free(cache->sets[i].fifo_queue);
        if (cache->sets[i].lru_deque) deque_free(cache->sets[i].lru_deque);
        if (cache->sets[i].lfu_heap) pq_free(cache->sets[i].lfu_heap);
    }
    
    free(cache->sets);
    if (cache->tag_table) hash_table_free(cache->tag_table);
    free(cache);
}
