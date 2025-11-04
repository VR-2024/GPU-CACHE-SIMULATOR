#include "deque.h"
#include <stdlib.h>
#include <limits.h>

deque_t* deque_create(void) {
    deque_t* d = (deque_t*)malloc(sizeof(deque_t));
    if (!d) return NULL;
    
    d->front = NULL;
    d->rear = NULL;
    d->size = 0;
    return d;
}

void deque_push_front(deque_t* d, uint32_t data) {
    if (!d) return;
    
    deque_node_t* node = (deque_node_t*)malloc(sizeof(deque_node_t));
    if (!node) return;
    
    node->data = data;
    node->next = d->front;
    node->prev = NULL;
    
    if (d->front != NULL) {
        d->front->prev = node;
    } else {
        d->rear = node;
    }
    
    d->front = node;
    d->size++;
}

uint32_t deque_pop_front(deque_t* d) {
    if (!d || d->front == NULL) return UINT32_MAX;
    
    deque_node_t* temp = d->front;
    uint32_t data = temp->data;
    d->front = d->front->next;
    
    if (d->front != NULL) {
        d->front->prev = NULL;
    } else {
        d->rear = NULL;
    }
    
    free(temp);
    d->size--;
    return data;
}

uint32_t deque_pop_back(deque_t* d) {
    if (!d || d->rear == NULL) return UINT32_MAX;
    
    deque_node_t* temp = d->rear;
    uint32_t data = temp->data;
    d->rear = d->rear->prev;
    
    if (d->rear != NULL) {
        d->rear->next = NULL;
    } else {
        d->front = NULL;
    }
    
    free(temp);
    d->size--;
    return data;
}

void deque_move_to_front(deque_t* d, uint32_t data) {
    if (!d || d->size < 2) return;
    
    /* Find the node */
    deque_node_t* node = d->front;
    while (node && node->data != data) {
        node = node->next;
    }
    
    if (!node || node == d->front) return;
    
    /* Remove from current position */
    if (node == d->rear) {
        d->rear = node->prev;
        d->rear->next = NULL;
    } else {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }
    
    /* Add to front */
    node->next = d->front;
    node->prev = NULL;
    d->front->prev = node;
    d->front = node;
}

bool deque_is_empty(deque_t* d) {
    return d == NULL || d->size == 0;
}

void deque_free(deque_t* d) {
    if (!d) return;
    
    while (!deque_is_empty(d)) {
        deque_pop_front(d);
    }
    free(d);
}

bool deque_contains(deque_t* d, uint32_t data) {
    if (!d) return false;
    
    deque_node_t* node = d->front;
    while (node) {
        if (node->data == data) return true;
        node = node->next;
    }
    return false;
}
