#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include "allocator.h"
#include "sys_alloc.h"
#include "linear_allocator.h"
#include "pool_allocator.h"
#include "buddy_allocator.h"
#include "arraylist.h"
#include "queue.h"
#include "hashtable.h"

int main(void) {
    printf("=== ArrayList тест (System Allocator) ===\n");
    {
        IAllocator sys = create_sys_alloc();
        ArrayList list;
        arraylist_init(&list, &sys, 2);

        int a = 1, b = 2, c = 3, d = 4;
        assert(arraylist_push(&list, &a));
        assert(arraylist_push(&list, &b));
        assert(arraylist_push(&list, &c));
        assert(arraylist_push(&list, &d));

        assert(*(int*)arraylist_get(&list, 0) == 1);
        assert(*(int*)arraylist_get(&list, 3) == 4);
        assert(arraylist_size(&list) == 4);

        arraylist_destroy(&list);
        printf("ArrayList: OK\n");
    }

    printf("\n=== ArrayList тест (Linear Allocator) ===\n");
    {
        char buffer[4096];
        LinearCtx ctx;
        IAllocator linear = create_linear_alloc(&ctx, buffer, sizeof(buffer));

        ArrayList list;
        arraylist_init(&list, &linear, 2);

        for (int i = 0; i < 100; i++) {
            int* val = (int*)i_alloc(&linear, sizeof(int));
            *val = i;
            arraylist_push(&list, val);
        }

        assert(arraylist_size(&list) == 100);
        assert(*(int*)arraylist_get(&list, 50) == 50);

        i_reset(&linear);
        printf("ArrayList (Linear): OK\n");
    }

    printf("\n=== Queue тест (Pool Allocator) ===\n");
    {
        char buffer[4096];
        PoolCtx ctx;
        IAllocator pool = create_pool_alloc(&ctx, buffer, sizeof(buffer), sizeof(QueueNode));

        Queue q;
        queue_init(&q, &pool);

        int vals[10];
        for (int i = 0; i < 10; i++) {
            vals[i] = i * 10;
            queue_push(&q, &vals[i]);
        }

        assert(queue_size(&q) == 10);
        assert(*(int*)queue_pop(&q) == 0);
        assert(*(int*)queue_pop(&q) == 10);
        assert(queue_size(&q) == 8);

        for (int i = 0; i < 1000; i++) {
            queue_push(&q, &vals[0]);
            queue_pop(&q);
        }
        assert(queue_size(&q) == 8);

        queue_destroy(&q);
        printf("Queue (Pool): OK\n");
    }

    printf("\n=== HashTable тест (Buddy Allocator) ===\n");
    {
        char raw_buffer[4096];
        uintptr_t addr = (uintptr_t)raw_buffer;
        uintptr_t aligned_addr = (addr + 1023) & ~((uintptr_t)1023);
        char* buffer = (char*)aligned_addr;
        size_t size = 1024;

        assert((char*)raw_buffer + sizeof(raw_buffer) >= buffer + size);
        assert(((uintptr_t)buffer & (size - 1)) == 0);

        BuddyCtx ctx;
        uint8_t orders[BUDDY_ORDERS_COUNT(1024, 16)];
        IAllocator buddy = create_buddy_alloc(&ctx, buffer, size, 16, orders);
        assert(ctx.buffer != NULL);

        HashTable ht;
        hashtable_init(&ht, &buddy, 8);

        hashtable_insert(&ht, "key1", (void*)100);
        hashtable_insert(&ht, "key2", (void*)200);
        hashtable_insert(&ht, "key3", (void*)300);

        hashtable_insert(&ht, "abc", (void*)1);
        hashtable_insert(&ht, "cba", (void*)2);
        hashtable_insert(&ht, "bca", (void*)3);

        assert((size_t)hashtable_get(&ht, "key1") == 100);
        assert((size_t)hashtable_get(&ht, "key2") == 200);
        assert((size_t)hashtable_get(&ht, "abc") == 1);
        assert((size_t)hashtable_get(&ht, "cba") == 2);

        hashtable_insert(&ht, "key1", (void*)999);
        assert((size_t)hashtable_get(&ht, "key1") == 999);

        assert(hashtable_remove(&ht, "key2"));
        assert(hashtable_get(&ht, "key2") == NULL);

        assert(hashtable_size(&ht) == 5);

        hashtable_destroy(&ht);
        printf("HashTable (Buddy): OK\n");
    }

    printf("\n=== Все тесты структур данных пройдены! ===\n");
    return 0;
}