#include "allocator.h"

void stub_free(IAllocator* self, void* ptr) {
    (void)self; 
    (void)ptr;
}

void* stub_realloc(IAllocator* self, void* ptr, size_t size) {
    (void)self; 
    (void)ptr; 
    (void)size;
    return NULL; 
}

void stub_reset(IAllocator* self) {
    (void)self;
}
