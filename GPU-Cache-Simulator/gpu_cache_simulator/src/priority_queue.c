#include "priority_queue.h"
#include <stdlib.h>
#include <limits.h>

priority_queue_t* pq_create(uint32_t capacity) {
    priority_queue_t* pq = (priority_queue_t*)malloc(sizeof(priority_queue_t));
    if (!pq) return NULL;
    
    // We use a 1-based index approach internally, so we allocate +1
    pq->heap = (pq_node_t*)malloc(sizeof(pq_node_t) * capacity);
    if (!pq->heap) {
        free(pq);
        return NULL;
    }
    
    pq->capacity = capacity;
    pq->size = 0;
    return pq;
}

static void swap_nodes(pq_node_t* a, pq_node_t* b) {
    pq_node_t temp = *a;
    *a = *b;
    *b = temp;
}

static void heapify_up(priority_queue_t* pq, uint32_t idx) {
    if (idx == 0) return;
    
    uint32_t parent = (idx - 1) / 2;
    
    if (pq->heap[idx].priority < pq->heap[parent].priority) {
        swap_nodes(&pq->heap[idx], &pq->heap[parent]);
        heapify_up(pq, parent);
    }
}

static void heapify_down(priority_queue_t* pq, uint32_t idx) {
    uint32_t smallest = idx;
    uint32_t left = 2 * idx + 1;
    uint32_t right = 2 * idx + 2;
    
    if (left < pq->size && pq->heap[left].priority < pq->heap[smallest].priority) {
        smallest = left;
    }
    
    if (right < pq->size && pq->heap[right].priority < pq->heap[smallest].priority) {
        smallest = right;
    }
    
    if (smallest != idx) {
        swap_nodes(&pq->heap[idx], &pq->heap[smallest]);
        heapify_down(pq, smallest);
    }
}

void pq_insert(priority_queue_t* pq, uint32_t data, uint32_t priority) {
    if (!pq || pq->size >= pq->capacity) return;
    
    uint32_t idx = pq->size;
    pq->heap[idx].data = data;
    pq->heap[idx].priority = priority;
    pq->size++;
    heapify_up(pq, idx);
}

uint32_t pq_extract_min(priority_queue_t* pq) {
    if (!pq || pq->size == 0) return UINT32_MAX;
    
    uint32_t data = pq->heap[0].data;
    pq->heap[0] = pq->heap[pq->size - 1];
    pq->size--;
    
    if (pq->size > 0) {
        heapify_down(pq, 0);
    }
    
    return data;
}

uint32_t pq_peek_min(priority_queue_t* pq) {
    if (!pq || pq->size == 0) return UINT32_MAX;
    return pq->heap[0].data;
}

bool pq_is_empty(priority_queue_t* pq) {
    return pq == NULL || pq->size == 0;
}

void pq_free(priority_queue_t* pq) {
    if (!pq) return;
    
    free(pq->heap);
    free(pq);
}
