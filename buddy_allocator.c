#include "buddy_allocator.h"
#include <string.h>
#include <stdint.h>

/* Округление размера до ближайшей степени двойки, начиная с min_size */
static int size_to_order(size_t size, size_t min_size, int min_order) {
    if (size <= min_size) return min_order;
    int order = min_order;
    size_t s = min_size;
    while (s < size) {
        s <<= 1;
        order++;
    }
    return order;
}

/* Индекс min-блока по адресу */
static size_t idx_from_ptr(BuddyCtx* ctx, void* ptr) {
    return ((uintptr_t)ptr - (uintptr_t)ctx->buffer) >> ctx->min_order;
}

static void* buddy_alloc_impl(IAllocator* self, size_t size) {
    BuddyCtx* ctx = (BuddyCtx*)self->ctx;
    if (!ctx || !ctx->buffer || size == 0) return NULL;

    int order = size_to_order(size, ctx->min_size, ctx->min_order);
    if (order > ctx->max_order) return NULL;

    /* Ищем первый непустой список подходящего или большего размера */
    int current_order = order;
    while (current_order <= ctx->max_order && !ctx->free_lists[current_order]) {
        current_order++;
    }
    if (current_order > ctx->max_order) return NULL;

    /* Забираем блок из списка */
    BuddyBlock* block = ctx->free_lists[current_order];
    ctx->free_lists[current_order] = block->next;

    /* Дробим до нужного порядка */
    while (current_order > order) {
        current_order--;
        size_t half_size = (size_t)1 << current_order;
        void* buddy = (void*)(((uintptr_t)block) + half_size);

        BuddyBlock* buddy_node = (BuddyBlock*)buddy;
        buddy_node->next = ctx->free_lists[current_order];
        ctx->free_lists[current_order] = buddy_node;
    }

    /* Запоминаем порядок выделенного блока */
    size_t idx = idx_from_ptr(ctx, block);
    ctx->orders[idx] = (uint8_t)order;
    return block;
}

static void buddy_free_impl(IAllocator* self, void* ptr) {
    if (!ptr) return;
    BuddyCtx* ctx = (BuddyCtx*)self->ctx;

    size_t idx = idx_from_ptr(ctx, ptr);
    int order = ctx->orders[idx];
    if (order < ctx->min_order || order > ctx->max_order) return;

    ctx->orders[idx] = 0xFF; /* Помечаем как свободный */

    void* block = ptr;
    while (order <= ctx->max_order) {
        size_t block_size = (size_t)1 << order;
        /* Находим адрес напарника через XOR */
        void* buddy = (void*)(((uintptr_t)block) ^ block_size);

        /* Ищем напарника в свободном списке этого порядка */
        BuddyBlock** current = &ctx->free_lists[order];
        int found = 0;
        while (*current) {
            if (*current == buddy) {
                *current = (*current)->next; /* Удаляем из списка */
                found = 1;
                break;
            }
            current = &(*current)->next;
        }

        if (!found) break; /* Напарник занят — останавливаем слияние */

        /* Сливаем: новый блок начинается с меньшего адреса */
        if (buddy < block) block = buddy;
        order++;
    }

    /* Вставляем итоговый (возможно, слитый) блок в список */
    BuddyBlock* node = (BuddyBlock*)block;
    node->next = ctx->free_lists[order];
    ctx->free_lists[order] = node;
}

static void* buddy_realloc_impl(IAllocator* self, void* ptr, size_t new_size) {
    BuddyCtx* ctx = (BuddyCtx*)self->ctx;
    if (!ptr) return buddy_alloc_impl(self, new_size);
    if (new_size == 0) {
        buddy_free_impl(self, ptr);
        return NULL;
    }

    size_t idx = idx_from_ptr(ctx, ptr);
    int old_order = ctx->orders[idx];
    size_t old_size = (size_t)1 << old_order;

    int new_order = size_to_order(new_size, ctx->min_size, ctx->min_order);
    if (new_order <= old_order) {
        /* Текущий блок уже достаточно велик */
        return ptr;
    }

    /* Выделяем новый блок, копируем данные, освобождаем старый */
    void* new_ptr = buddy_alloc_impl(self, new_size);
    if (!new_ptr) return NULL;

    size_t copy_size = old_size < new_size ? old_size : new_size;
    memcpy(new_ptr, ptr, copy_size);
    buddy_free_impl(self, ptr);
    return new_ptr;
}

static void buddy_reset_impl(IAllocator* self) {
    BuddyCtx* ctx = (BuddyCtx*)self->ctx;
    if (!ctx) return;

    for (int i = 0; i < 64; i++) {
        ctx->free_lists[i] = NULL;
    }

    size_t num_blocks = ctx->size / ctx->min_size;
    for (size_t i = 0; i < num_blocks; i++) {
        ctx->orders[i] = 0xFF;
    }

    ctx->free_lists[ctx->max_order] = (BuddyBlock*)ctx->buffer;
    if (ctx->buffer) ctx->free_lists[ctx->max_order]->next = NULL;
}

IAllocator create_buddy_alloc(BuddyCtx* ctx, void* buffer, size_t size,
                              size_t min_size, uint8_t* orders) {
    memset(ctx, 0, sizeof(BuddyCtx));
    ctx->buffer   = buffer;
    ctx->size     = size;
    ctx->min_size = min_size;
    ctx->orders   = orders;

    /* Валидация: size и min_size — степени двойки, size >= min_size */
    if ((size & (size - 1)) != 0 || (min_size & (min_size - 1)) != 0 || size < min_size) {
        ctx->buffer = NULL;
    }
    /* Валидация выравнивания буфера (критично для XOR) */
    if (((uintptr_t)buffer & (size - 1)) != 0) {
        ctx->buffer = NULL;
    }
    /* Минимальный блок должен вмещать указатель для free-list */
    if (min_size < sizeof(BuddyBlock)) {
        ctx->buffer = NULL;
    }

    /* Вычисляем порядки */
    ctx->min_order = 0;
    size_t s = min_size;
    while (s > 1) { s >>= 1; ctx->min_order++; }

    ctx->max_order = 0;
    s = size;
    while (s > 1) { s >>= 1; ctx->max_order++; }

    /* Инициализация orders и единого свободного блока */
    size_t num_blocks = size / min_size;
    for (size_t i = 0; i < num_blocks; i++) {
        orders[i] = 0xFF;
    }

    ctx->free_lists[ctx->max_order] = (BuddyBlock*)buffer;
    if (buffer) ctx->free_lists[ctx->max_order]->next = NULL;

    IAllocator allocator = {
        .alloc   = buddy_alloc_impl,
        .free    = buddy_free_impl,
        .realloc = buddy_realloc_impl,
        .reset   = buddy_reset_impl,
        .ctx     = ctx
    };
    return allocator;
}