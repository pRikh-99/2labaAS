#include "sys_alloc.h"
#include <stdlib.h>

static void* sys_alloc_impl(IAllocator* self, size_t size) {
    (void)self; 
    return malloc(size);
}

static void sys_free_impl(IAllocator* self, void* ptr) {
    (void)self;
    free(ptr);
}

static void* sys_realloc_impl(IAllocator* self, void* ptr, size_t new_size) {
    (void)self;
    return realloc(ptr, new_size);
}

static void sys_reset_impl(IAllocator* self) {
    (void)self;
}

IAllocator create_sys_alloc(void) {
    IAllocator allocator = {
        .alloc = sys_alloc_impl,
        .free = sys_free_impl,
        .realloc = sys_realloc_impl,
        .reset = sys_reset_impl,
        .ctx = NULL
    };
    return allocator;
}
