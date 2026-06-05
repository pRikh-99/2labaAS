#ifndef ARRAYLIST_H
#define ARRAYLIST_H

#include "allocator.h"
#include <stddef.h>

typedef struct {
    IAllocator* alloc;
    void** data;
    size_t size;
    size_t capacity;
} ArrayList;

void arraylist_init(ArrayList* list, IAllocator* alloc, size_t initial_capacity);
void arraylist_destroy(ArrayList* list);
int arraylist_push(ArrayList* list, void* item);
void* arraylist_pop(ArrayList* list);
void* arraylist_get(ArrayList* list, size_t index);
int arraylist_set(ArrayList* list, size_t index, void* item);
void arraylist_clear(ArrayList* list);
size_t arraylist_size(const ArrayList* list);

#endif