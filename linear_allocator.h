#ifndef LINEAR_ALLOCATOR_H
#define LINEAR_ALLOCATOR_H

#include "allocator.h"

typedef struct {
    void*  buffer;   /* Начало выделенной области */
    size_t size;     /* Общий размер */
    size_t offset;   /* Текущая позиция */
} LinearCtx;

/* Конструктор. Пользователь должен обеспечить хранение ctx */
IAllocator create_linear_alloc(LinearCtx* ctx, void* buffer, size_t size);

#endif