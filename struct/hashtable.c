#include "hashtable.h"
#include <string.h>

static size_t hash(const char* key, size_t bucket_count) {
    size_t h = 0;
    while (*key) {
        h = h * 31 + *key;
        key++;
    }
    return h % bucket_count;
}

void hashtable_init(HashTable* t, IAllocator* alloc, size_t bucket_count) {
    t->alloc = alloc;
    t->bucket_count = bucket_count;
    t->buckets = i_alloc(alloc, bucket_count * sizeof(HashNode*));
    for (size_t i = 0; i < bucket_count; i++) {
        t->buckets[i] = NULL;
    }
}

void hashtable_insert(HashTable* t, const char* key, void* value) {
    size_t idx = hash(key, t->bucket_count);
    HashNode* node = i_alloc(t->alloc, sizeof(HashNode));
    node->key = key;
    node->value = value;
    node->next = t->buckets[idx];
    t->buckets[idx] = node;
}

void* hashtable_get(HashTable* t, const char* key) {
    size_t idx = hash(key, t->bucket_count);
    HashNode* n = t->buckets[idx];
    while (n) {
        if (strcmp(n->key, key) == 0) return n->value;
        n = n->next;
    }
    return NULL;
}

void hashtable_remove(HashTable* t, const char* key) {
    size_t idx = hash(key, t->bucket_count);
    HashNode* prev = NULL;
    HashNode* n = t->buckets[idx];
    while (n) {
        if (strcmp(n->key, key) == 0) {
            if (prev) prev->next = n->next;
            else t->buckets[idx] = n->next;
            i_free(t->alloc, n);
            return;
        }
        prev = n;
        n = n->next;
    }
}

void hashtable_destroy(HashTable* t) {
    for (size_t i = 0; i < t->bucket_count; i++) {
        HashNode* n = t->buckets[i];
        while (n) {
            HashNode* tmp = n;
            n = n->next;
            i_free(t->alloc, tmp);
        }
    }
    i_free(t->alloc, t->buckets);
}