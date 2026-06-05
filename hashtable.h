#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "allocator.h"
#include <stddef.h>

typedef struct HashNode {
    char* key;
    void* value;
    struct HashNode* next;
} HashNode;

typedef struct {
    IAllocator* alloc;
    HashNode** buckets;
    size_t capacity;
    size_t size;
} HashTable;

void hashtable_init(HashTable* ht, IAllocator* alloc, size_t capacity);
void hashtable_destroy(HashTable* ht);
int hashtable_insert(HashTable* ht, const char* key, void* value);
void* hashtable_get(HashTable* ht, const char* key);
int hashtable_remove(HashTable* ht, const char* key);
void hashtable_clear(HashTable* ht);
size_t hashtable_size(const HashTable* ht);

#endif