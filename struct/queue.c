#include "queue.h"

void queue_init(Queue* q, IAllocator* alloc) {
    q->alloc = alloc;
    q->head = NULL;
    q->tail = NULL;
}

void queue_push(Queue* q, void* value) {
    Node* node = i_alloc(q->alloc, sizeof(Node));
    node->data = value;
    node->next = NULL;
    if (q->tail) {
        q->tail->next = node;
    } else {
        q->head = node;
    }
    q->tail = node;
}

void* queue_pop(Queue* q) {
    if (!q->head) return NULL;
    Node* node = q->head;
    void* data = node->data;
    q->head = node->next;
    if (!q->head) q->tail = NULL;
    i_free(q->alloc, node);
    return data;
}

int queue_is_empty(Queue* q) {
    return q->head == NULL;
}

void queue_destroy(Queue* q) {
    while (!queue_is_empty(q)) {
        queue_pop(q);
    }
}