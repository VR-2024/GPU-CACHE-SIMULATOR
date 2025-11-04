#include "hash_table.h"
#include <stdlib.h>
#include <limits.h>

hash_table_t* hash_table_create(uint32_t capacity) {
    hash_table_t* ht = (hash_table_t*)malloc(sizeof(hash_table_t));
    if (!ht) return NULL;
    
    ht->buckets = (hash_node_t**)calloc(capacity, sizeof(hash_node_t*));
    if (!ht->buckets) {
        free(ht);
        return NULL;
    }
    
    ht->capacity = capacity;
    ht->size = 0;
    return ht;
}

static uint32_t hash_function(uint64_t key, uint32_t capacity) {
    // Simple hash function using bitwise XOR and modulo
    return (uint32_t)((key ^ (key >> 32)) % capacity);
}

void hash_table_insert(hash_table_t* ht, uint64_t key, uint32_t value) {
    if (!ht) return;
    
    uint32_t index = hash_function(key, ht->capacity);
    
    // Check if key already exists (update in place)
    hash_node_t* current = ht->buckets[index];
    while (current) {
        if (current->key == key) {
            current->value = value;
            return;
        }
        current = current->next;
    }
    
    // Key does not exist, insert new node
    hash_node_t* new_node = (hash_node_t*)malloc(sizeof(hash_node_t));
    if (!new_node) return;
    
    new_node->key = key;
    new_node->value = value;
    new_node->next = ht->buckets[index];
    
    ht->buckets[index] = new_node;
    ht->size++;
}

uint32_t hash_table_lookup(hash_table_t* ht, uint64_t key) {
    if (!ht) return UINT32_MAX;
    
    uint32_t index = hash_function(key, ht->capacity);
    hash_node_t* current = ht->buckets[index];
    
    while (current) {
        if (current->key == key) {
            return current->value;
        }
        current = current->next;
    }
    
    return UINT32_MAX; // Not found
}

void hash_table_delete(hash_table_t* ht, uint64_t key) {
    if (!ht) return;
    
    uint32_t index = hash_function(key, ht->capacity);
    hash_node_t* current = ht->buckets[index];
    hash_node_t* prev = NULL;
    
    while (current) {
        if (current->key == key) {
            if (prev) {
                prev->next = current->next;
            } else {
                ht->buckets[index] = current->next;
            }
            free(current);
            ht->size--;
            return;
        }
        prev = current;
        current = current->next;
    }
}

void hash_table_free(hash_table_t* ht) {
    if (!ht) return;
    
    for (uint32_t i = 0; i < ht->capacity; i++) {
        hash_node_t* current = ht->buckets[i];
        hash_node_t* next;
        while (current) {
            next = current->next;
            free(current);
            current = next;
        }
    }
    free(ht->buckets);
    free(ht);
}
