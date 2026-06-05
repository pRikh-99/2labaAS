#ifndef BUDDY_ALLOCATOR_H
#define BUDDY_ALLOCATOR_H

#include "allocator.h"
#include <stdint.h>

/* Узел свободного списка — хранится внутри свободного блока */
typedef struct BuddyBlock {
    struct BuddyBlock* next;
} BuddyBlock;

typedef struct {
    void*    buffer;         /* Начало пула, выровнено до размера пула */
    size_t   size;           /* Размер пула, степень двойки */
    size_t   min_size;       /* Минимальный размер блока, степень двойки */
    int      min_order;      /* log2(min_size) */
    int      max_order;      /* log2(size) */
    BuddyBlock* free_lists[64]; /* Свободные списки по порядкам */
    uint8_t* orders;         /* Массив: порядок блока для каждого min-блока */
} BuddyCtx;

/* Размер массива orders в байтах */
#define BUDDY_ORDERS_COUNT(buf_sz, min_sz) ((buf_sz) / (min_sz))

/*
 * Конструктор.
 *  - size и min_size должны быть степенями двойки.
 *  - buffer должен быть выровнен по границе size (необходимо для XOR).
 *  - orders — массив из BUDDY_ORDERS_COUNT(size, min_size) байт,
 *    выделенный вызывающей стороной.
 */
IAllocator create_buddy_alloc(BuddyCtx* ctx, void* buffer, size_t size,
                              size_t min_size, uint8_t* orders);

#endif