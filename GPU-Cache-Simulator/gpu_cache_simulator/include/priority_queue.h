#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint32_t data;     // Index of the cache block
    uint32_t priority; // Access count (for LFU) or other metric
} pq_node_t;

typedef struct {
    pq_node_t* heap;
    uint32_t size;
    uint32_t capacity;
} priority_queue_t;

priority_queue_t* pq_create(uint32_t capacity);
void pq_insert(priority_queue_t* pq, uint32_t data, uint32_t priority);
uint32_t pq_extract_min(priority_queue_t* pq);
uint32_t pq_peek_min(priority_queue_t* pq);
bool pq_is_empty(priority_queue_t* pq);
void pq_free(priority_queue_t* pq);

#endif // PRIORITY_QUEUE_H
