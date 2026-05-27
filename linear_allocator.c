#include "linear_allocator.h"

/* Выравнивание до 8 байт */
#define ALIGN8(x) (((x) + 7) & ~7)

static void* linear_alloc_impl(IAllocator* self, size_t size) {
    LinearCtx* ctx = (LinearCtx*)self->ctx;
    if (!ctx || !ctx->buffer || size == 0) return NULL;

    size_t aligned = ALIGN8(size);
    if (ctx->offset + aligned > ctx->size) return NULL;

    void* ptr = (char*)ctx->buffer + ctx->offset;
    ctx->offset += aligned;
    return ptr;
}

static void linear_free_impl(IAllocator* self, void* ptr) {
    (void)self;
    (void)ptr;
    /* Освобождение отдельных блоков не поддерживается */
}

static void linear_reset_impl(IAllocator* self) {
    LinearCtx* ctx = (LinearCtx*)self->ctx;
    if (ctx) ctx->offset = 0;
}

IAllocator create_linear_alloc(LinearCtx* ctx, void* buffer, size_t size) {
    ctx->buffer = buffer;
    ctx->size   = size;
    ctx->offset = 0;

    IAllocator allocator = {
        .alloc   = linear_alloc_impl,
        .free    = linear_free_impl,
        .realloc = stub_realloc,   /* Линейный аллокатор не умеет realloc */
        .reset   = linear_reset_impl,
        .ctx     = ctx
    };
    return allocator;
}