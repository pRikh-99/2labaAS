#ifndef POOL_ALLOCATOR_H
#define POOL_ALLOCATOR_H

#include "allocator.h"

/* Узок свободного списка — хранится внутри самого свободного блока */
typedef struct PoolNode {
    struct PoolNode* next;
} PoolNode;

/* Контекст пул-аллокатора */
typedef struct {
    void*    buffer;
    size_t   block_size;
    size_t   block_count;
    PoolNode* free_list;
} PoolCtx;

/* 
 * Конструктор. buffer_size должно быть кратно block_size (с учётом выравнивания).
 * block_size будет округлён вверх до sizeof(void*).
 */
IAllocator create_pool_alloc(PoolCtx* ctx, void* buffer, size_t buffer_size, size_t block_size);

#endif