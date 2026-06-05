#include "hashtable.h"
#include <string.h>

static unsigned long hash_string(const char* str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

void hashtable_init(HashTable* ht, IAllocator* alloc, size_t capacity) {
    ht->alloc = alloc;
    ht->capacity = capacity > 0 ? capacity : 16;
    ht->size = 0;
    ht->buckets = (HashNode**)i_alloc(alloc, ht->capacity * sizeof(HashNode*));
    if (ht->buckets) {
        memset(ht->buckets, 0, ht->capacity * sizeof(HashNode*));
    }
}

void hashtable_destroy(HashTable* ht) {
    hashtable_clear(ht);
    if (ht->buckets) {
        i_free(ht->alloc, ht->buckets);
        ht->buckets = NULL;
    }
    ht->capacity = 0;
}

int hashtable_insert(HashTable* ht, const char* key, void* value) {
    if (!ht->buckets) return 0;

    unsigned long hash = hash_string(key);
    size_t idx = hash % ht->capacity;

    HashNode* current = ht->buckets[idx];
    while (current) {
        if (strcmp(current->key, key) == 0) {
            current->value = value;
            return 1;
        }
        current = current->next;
    }

    HashNode* n = (HashNode*)i_alloc(ht->alloc, sizeof(HashNode));
    if (!n) return 0;

    size_t key_len = strlen(key) + 1;
    n->key = (char*)i_alloc(ht->alloc, key_len);
    if (!n->key) {
        i_free(ht->alloc, n);
        return 0;
    }
    memcpy(n->key, key, key_len);

    n->value = value;
    n->next = ht->buckets[idx];
    ht->buckets[idx] = n;
    ht->size++;
    return 1;
}

void* hashtable_get(HashTable* ht, const char* key) {
    if (!ht->buckets) return NULL;

    unsigned long hash = hash_string(key);
    size_t idx = hash % ht->capacity;

    HashNode* current = ht->buckets[idx];
    while (current) {
        if (strcmp(current->key, key) == 0) {
            return current->value;
        }
        current = current->next;
    }
    return NULL;
}

int hashtable_remove(HashTable* ht, const char* key) {
    if (!ht->buckets) return 0;

    unsigned long hash = hash_string(key);
    size_t idx = hash % ht->capacity;

    HashNode** current = &ht->buckets[idx];
    while (*current) {
        if (strcmp((*current)->key, key) == 0) {
            HashNode* to_remove = *current;
            *current = (*current)->next;

            i_free(ht->alloc, to_remove->key);
            i_free(ht->alloc, to_remove);
            ht->size--;
            return 1;
        }
        current = &(*current)->next;
    }
    return 0;
}

void hashtable_clear(HashTable* ht) {
    if (!ht->buckets) return;

    for (size_t i = 0; i < ht->capacity; i++) {
        HashNode* current = ht->buckets[i];
        while (current) {
            HashNode* next = current->next;
            i_free(ht->alloc, current->key);
            i_free(ht->alloc, current);
            current = next;
        }
        ht->buckets[i] = NULL;
    }
    ht->size = 0;
}

size_t hashtable_size(const HashTable* ht) {
    return ht->size;
}