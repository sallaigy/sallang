#include <stdlib.h>
#include <string.h>
#include "slg.h"
#include <common/hash.h>

HashTable *HashTable_create(size_t size)
{
    HashTable *ht = malloc(sizeof(HashTable));

    SLG_TRY(ht != NULL, MemoryError);

    ht->size    = size;
    ht->buckets = calloc(ht->size, sizeof(List));
    ht->dtor    = free;

    SLG_TRY(ht->buckets != NULL, MemoryError);

    return ht;

MemoryError:
    if (ht != NULL) {
        free(ht);
    }

    return NULL;
}

static inline List *HashTable_find_bucket(HashTable *ht, uint32_t hash, bool create)
{
    int bucket_n  = hash % ht->size;

    List *bucket  = ht->buckets[bucket_n];

    if (bucket == NULL && create) {
        bucket = List_create(free);
        ht->buckets[bucket_n] = bucket;
    }

    return bucket;
}

bool HashTable_set(HashTable *ht, const char *key, void *data)
{
    uint32_t hash = HashTable_hash(ht, key);
    List *bucket = HashTable_find_bucket(ht, hash, true);

    HashTableNode *node = malloc(sizeof(HashTableNode));

    if (node == NULL) {
        return false;
    }

    node->hash  = hash;
    node->key   = key;
    node->data  = data;

    List_append(bucket, node);

    return true;
}

void *HashTable_get(HashTable *ht, const char *key)
{
    uint32_t hash = HashTable_hash(ht, key);
    List *bucket = HashTable_find_bucket(ht, hash, false);

    if (bucket == NULL) { // No such bucket
        return NULL;
    }

    LIST_FOREACH(bucket, first, next, curr) {
        HashTableNode *node = (HashTableNode*) curr->data;
        if (node->hash == hash && strcmp(node->key, key) == 0) {
            return node->data;
        }
    }

    return NULL;
}

void HashTable_destroy(HashTable *ht)
{
    int i;


    for (i = 0; i < ht->size; i++) {
        List *bucket = ht->buckets[i];

        if (bucket == NULL) {
            continue;
        }

        LIST_FOREACH(bucket, first, next, curr) {
            HashTableNode *node = (HashTableNode*) curr->data;
            ht->dtor(node->data);
        }

        List_destroy(bucket);
    }

    free(ht->buckets);
    free(ht);
}

uint32_t HashTable_hash(HashTable *ht, const char *key)
{
    uint32_t hash, i;
    size_t len = strlen(key);

    for (hash = i = 0; i < len; ++i)
    {
        hash += key[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}