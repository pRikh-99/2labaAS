#ifndef ARRAYLIST_H
#define ARRAYLIST_H

#include "../allocator.h"
#include <stddef.h>

typedef struct {
    IAllocator* alloc;
    void** data;
    size_t size;
    size_t capacity;
} ArrayList;

void arraylist_init(ArrayList* list, IAllocator* alloc);
void arraylist_add(ArrayList* list, void* value);
void* arraylist_get(ArrayList* list, size_t index);
void arraylist_remove(ArrayList* list, size_t index);
void arraylist_destroy(ArrayList* list);

#endif