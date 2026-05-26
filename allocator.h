#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>

typedef struct IAllocator {
    void* (*alloc)(struct IAllocator* self, size_t size);
    void  (*free)(struct IAllocator* self, void* ptr);
    void* (*realloc)(struct IAllocator* self, void* ptr, size_t new_size);
    void  (*reset)(struct IAllocator* self);
    void* ctx;
} IAllocator;

void stub_free(IAllocator* self, void* ptr);
void* stub_realloc(IAllocator* self, void* ptr, size_t size);
void stub_reset(IAllocator* self);

#endif
