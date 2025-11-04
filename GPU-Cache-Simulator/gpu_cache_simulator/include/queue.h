#ifndef QUEUE_H
#define QUEUE_H

#include <stdint.h>
#include <stdbool.h>

typedef struct queue_node_t {
    uint32_t data; // Index of the cache block
    struct queue_node_t* next;
} queue_node_t;

typedef struct {
    queue_node_t* front;
    queue_node_t* rear;
    uint32_t size;
} queue_t;

queue_t* queue_create(void);
void queue_enqueue(queue_t* q, uint32_t data);
uint32_t queue_dequeue(queue_t* q);
bool queue_is_empty(queue_t* q);
void queue_free(queue_t* q);

#endif // QUEUE_H
