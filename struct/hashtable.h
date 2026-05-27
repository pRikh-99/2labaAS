#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "../allocator.h"

typedef struct HashNode {
    const char* key;
    void* value;
    struct HashNode* next;
} HashNode;

typedef struct {
    IAllocator* alloc;
    HashNode** buckets;
    size_t bucket_count;
} HashTable;

void hashtable_init(HashTable* t, IAllocator* alloc, size_t bucket_count);
void hashtable_insert(HashTable* t, const char* key, void* value);
void* hashtable_get(HashTable* t, const char* key);
void hashtable_remove(HashTable* t, const char* key);
void hashtable_destroy(HashTable* t);

#endif