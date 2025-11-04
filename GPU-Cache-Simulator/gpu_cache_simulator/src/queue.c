#include "queue.h"
#include <stdlib.h>
#include <limits.h>

queue_t* queue_create(void) {
    queue_t* q = (queue_t*)malloc(sizeof(queue_t));
    if (!q) return NULL;
    
    q->front = NULL;
    q->rear = NULL;
    q->size = 0;
    return q;
}

void queue_enqueue(queue_t* q, uint32_t data) {
    if (!q) return;
    
    queue_node_t* node = (queue_node_t*)malloc(sizeof(queue_node_t));
    if (!node) return;
    
    node->data = data;
    node->next = NULL;
    
    if (q->rear != NULL) {
        q->rear->next = node;
    } else {
        q->front = node;
    }
    
    q->rear = node;
    q->size++;
}

uint32_t queue_dequeue(queue_t* q) {
    if (!q || q->front == NULL) return UINT32_MAX;
    
    queue_node_t* temp = q->front;
    uint32_t data = temp->data;
    q->front = q->front->next;
    
    if (q->front == NULL) {
        q->rear = NULL;
    }
    
    free(temp);
    q->size--;
    return data;
}

bool queue_is_empty(queue_t* q) {
    return q == NULL || q->front == NULL;
}

void queue_free(queue_t* q) {
    if (!q) return;
    
    while (!queue_is_empty(q)) {
        queue_dequeue(q);
    }
    free(q);
}
