#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdint.h>
#include <stdbool.h>

typedef struct hash_node_t {
    uint64_t key;
    uint32_t value; // Index of the cache block in the set
    struct hash_node_t* next;
} hash_node_t;

typedef struct {
    hash_node_t** buckets;
    uint32_t size;
    uint32_t capacity;
} hash_table_t;

hash_table_t* hash_table_create(uint32_t capacity);
void hash_table_insert(hash_table_t* ht, uint64_t key, uint32_t value);
uint32_t hash_table_lookup(hash_table_t* ht, uint64_t key);
void hash_table_delete(hash_table_t* ht, uint64_t key);
void hash_table_free(hash_table_t* ht);

#endif // HASH_TABLE_H
