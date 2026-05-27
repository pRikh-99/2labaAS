#include "pool_allocator.h"

#define ALIGN8(x) (((x) + 7) & ~7)

static void* pool_alloc_impl(IAllocator* self, size_t size) {
    PoolCtx* ctx = (PoolCtx*)self->ctx;
    if (!ctx || size > ctx->block_size) return NULL;
    if (!ctx->free_list) return NULL;

    PoolNode* node = ctx->free_list;
    ctx->free_list = node->next;
    return node;
}

static void pool_free_impl(IAllocator* self, void* ptr) {
    if (!ptr) return;
    PoolCtx* ctx = (PoolCtx*)self->ctx;
    PoolNode* node = (PoolNode*)ptr;
    node->next = ctx->free_list;
    ctx->free_list = node;
}

static void* pool_realloc_impl(IAllocator* self, void* ptr, size_t new_size) {
    PoolCtx* ctx = (PoolCtx*)self->ctx;
    if (!ptr) return pool_alloc_impl(self, new_size);
    if (new_size == 0) {
        pool_free_impl(self, ptr);
        return NULL;
    }
    /* Если новый размер помещается в существующий блок — оставляем как есть */
    if (new_size <= ctx->block_size) return ptr;
    return NULL;
}

static void pool_reset_impl(IAllocator* self) {
    PoolCtx* ctx = (PoolCtx*)self->ctx;
    if (!ctx || !ctx->buffer) return;

    ctx->free_list = NULL;
    char* ptr = (char*)ctx->buffer;
    for (size_t i = 0; i < ctx->block_count; i++) {
        PoolNode* node = (PoolNode*)ptr;
        node->next = ctx->free_list;
        ctx->free_list = node;
        ptr += ctx->block_size;
    }
}

IAllocator create_pool_alloc(PoolCtx* ctx, void* buffer, size_t buffer_size, size_t block_size) {
    if (block_size < sizeof(PoolNode)) block_size = sizeof(PoolNode);
    block_size = ALIGN8(block_size);

    ctx->buffer      = buffer;
    ctx->block_size  = block_size;
    ctx->block_count = buffer_size / block_size;
    ctx->free_list   = NULL;

    char* ptr = (char*)buffer;
    for (size_t i = 0; i < ctx->block_count; i++) {
        PoolNode* node = (PoolNode*)ptr;
        node->next = ctx->free_list;
        ctx->free_list = node;
        ptr += block_size;
    }

    IAllocator allocator = {
        .alloc   = pool_alloc_impl,
        .free    = pool_free_impl,
        .realloc = pool_realloc_impl,
        .reset   = pool_reset_impl,
        .ctx     = ctx
    };
    return allocator;
}