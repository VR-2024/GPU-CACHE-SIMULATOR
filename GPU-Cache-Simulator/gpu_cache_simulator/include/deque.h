#ifndef DEQUE_H
#define DEQUE_H

#include <stdint.h>
#include <stdbool.h>

typedef struct deque_node_t {
    uint32_t data; // Index of the cache block
    struct deque_node_t* next;
    struct deque_node_t* prev;
} deque_node_t;

typedef struct {
    deque_node_t* front;
    deque_node_t* rear;
    uint32_t size;
} deque_t;

deque_t* deque_create(void);
void deque_push_front(deque_t* d, uint32_t data);
uint32_t deque_pop_front(deque_t* d);
uint32_t deque_pop_back(deque_t* d);
void deque_move_to_front(deque_t* d, uint32_t data); // Key operation for LRU
bool deque_is_empty(deque_t* d);
void deque_free(deque_t* d);
bool deque_contains(deque_t* d, uint32_t data);

#endif // DEQUE_H
