#include "queue.h"

void queue_init(Queue* q, IAllocator* alloc) {
    q->alloc = alloc;
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
}

void queue_destroy(Queue* q) {
    queue_clear(q);
}

int queue_push(Queue* q, void* val) {
    QueueNode* n = (QueueNode*)i_alloc(q->alloc, sizeof(QueueNode));
    if (!n) return 0;

    n->data = val;
    n->next = NULL;

    if (q->tail) {
        q->tail->next = n;
    } else {
        q->head = n;
    }
    q->tail = n;
    q->size++;
    return 1;
}

void* queue_pop(Queue* q) {
    if (!q->head) return NULL;

    QueueNode* n = q->head;
    void* val = n->data;

    q->head = n->next;
    if (!q->head) {
        q->tail = NULL;
    }
    q->size--;

    i_free(q->alloc, n);
    return val;
}

void* queue_peek(Queue* q) {
    if (!q->head) return NULL;
    return q->head->data;
}

void queue_clear(Queue* q) {
    while (q->head) {
        queue_pop(q);
    }
}

size_t queue_size(const Queue* q) {
    return q->size;
}