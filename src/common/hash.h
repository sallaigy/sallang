#ifndef _SALLANG_HASH_H
#define _SALLANG_HASH_H

#include "slg.h"
#include <common/list.h>

typedef struct _HashTableNode {
    uint32_t hash;
    const char *key;

    void *data;
} HashTableNode;

typedef void (*HashTable_dtor_func_t)(void *);

typedef struct _HashTable {

    size_t size;

    // This is an array of linked lists.
    List **buckets;

    HashTable_dtor_func_t dtor;

} HashTable;

HashTable *HashTable_create(size_t size);

uint32_t HashTable_hash(HashTable *ht, const char *key);

bool HashTable_set(HashTable *ht, const char *key, void *data);
void *HashTable_get(HashTable *ht, const char *key);

void HashTable_destroy(HashTable *ht);


#endif