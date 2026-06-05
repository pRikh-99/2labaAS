#ifndef QUEUE_H
#define QUEUE_H

#include "allocator.h"
#include <stddef.h>

typedef struct QueueNode {
    void* data;
    struct QueueNode* next;
} QueueNode;

typedef struct {
    IAllocator* alloc;
    QueueNode* head;
    QueueNode* tail;
    size_t size;
} Queue;

void queue_init(Queue* q, IAllocator* alloc);
void queue_destroy(Queue* q);
int queue_push(Queue* q, void* val);
void* queue_pop(Queue* q);
void* queue_peek(Queue* q);
void queue_clear(Queue* q);
size_t queue_size(const Queue* q);

#endif