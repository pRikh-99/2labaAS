#include <stdio.h>
#include <assert.h>
#include "allocator.h"
#include "sys_alloc.h"
#include "struct/arraylist.h"
#include "struct/queue.h"
#include "struct/hashtable.h"

int main(void) {
    IAllocator alloc = create_sys_alloc();
    //ArrayList
    printf("    ArrayList\n");
    ArrayList list;
    arraylist_init(&list, &alloc);

    int a = 1, b = 2, c = 3;
    arraylist_add(&list, &a);
    arraylist_add(&list, &b);
    arraylist_add(&list, &c);

    assert(arraylist_get(&list, 0) == &a);
    assert(arraylist_get(&list, 1) == &b);
    assert(arraylist_get(&list, 2) == &c);
    assert(list.size == 3);

    arraylist_remove(&list, 1);
    assert(arraylist_get(&list, 1) == &c);
    assert(list.size == 2);

    arraylist_destroy(&list);
    printf("ArrayList: OK");

    // --- Queue ---
    printf("  Queue\n");
    Queue q;
    queue_init(&q, &alloc);

    int x = 10, y = 20, z = 30;
    queue_push(&q, &x);
    queue_push(&q, &y);
    queue_push(&q, &z);

    assert(queue_pop(&q) == &x);
    assert(queue_pop(&q) == &y);
    assert(queue_pop(&q) == &z);
    assert(queue_is_empty(&q));

    queue_destroy(&q);
    printf("Queue: OK\n");

    // --- HashTable ---
    printf("   HashTable\n");
    HashTable t;
    hashtable_init(&t, &alloc, 8);

    int v1 = 100, v2 = 200;
    hashtable_insert(&t, "key1", &v1);
    hashtable_insert(&t, "key2", &v2);

    assert(hashtable_get(&t, "key1") == &v1);
    assert(hashtable_get(&t, "key2") == &v2);
    assert(hashtable_get(&t, "key3") == NULL);

    hashtable_remove(&t, "key1");
    assert(hashtable_get(&t, "key1") == NULL);
    assert(hashtable_get(&t, "key2") == &v2);

    hashtable_destroy(&t);
    printf("HashTable: OK\n");

    printf("   Все тесты структур пройдены!");
    return 0;
}