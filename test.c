#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include "allocator.h"
#include "sys_alloc.h"
#include "linear_allocator.h"
#include "pool_allocator.h"
#include "buddy_allocator.h"

int main(void) {
    printf("=== System-аллокатор тест ===\n");
    {
        IAllocator sys = create_sys_alloc();
        void* p1 = i_alloc(&sys, 100);
        assert(p1 != NULL);
        memset(p1, 0xAB, 100);

        void* p2 = i_realloc(&sys, p1, 200);
        assert(p2 != NULL);
        i_free(&sys, p2);
        i_reset(&sys); /* no-op */
        printf("System-аллокатор: OK\n");
    }

    printf("\n=== Linear-аллокатор тест ===\n");
    {
        char buffer[1024];
        LinearCtx ctx;
        IAllocator linear = create_linear_alloc(&ctx, buffer, sizeof(buffer));

        void* p1 = i_alloc(&linear, 100);
        void* p2 = i_alloc(&linear, 50);
        assert(p1 && p2);
        assert((char*)p2 == (char*)p1 + 104); /* 100 выровнено до 104 */

        void* p3 = i_alloc(&linear, 900); /* Не поместится */
        assert(p3 == NULL);

        i_reset(&linear);
        void* p4 = i_alloc(&linear, 200);
        assert(p4 == buffer); /* С начала после сброса */

        i_free(&linear, p1); /* Безопасная заглушка */
        printf("Linear-аллокатор: OK\n");
    }

    printf("\n=== Pool-аллокатор тест ===\n");
    {
        char buffer[1024];
        PoolCtx ctx;
        IAllocator pool = create_pool_alloc(&ctx, buffer, sizeof(buffer), 64);
        printf("Pool config: block_size=%zu, block_count=%zu\n", ctx.block_size, ctx.block_count);

        void* p1 = i_alloc(&pool, 64);
        void* p2 = i_alloc(&pool, 64);
        void* p3 = i_alloc(&pool, 64);
        assert(p1 && p2 && p3);

        i_free(&pool, p2);
        void* p4 = i_alloc(&pool, 64);
        assert(p4 == p2); /* Переиспользуем освобождённый блок */

        void* p5 = i_alloc(&pool, 100); /* Слишком большой запрос */
        assert(p5 == NULL);

        void* p6 = i_realloc(&pool, p1, 32); /* Влезает в 64-байтный блок */
        assert(p6 == p1);

        void* p7 = i_realloc(&pool, p1, 100); /* Не влезает */
        assert(p7 == NULL);

        i_reset(&pool);
        void* p8 = i_alloc(&pool, 64);
        assert(p8 != NULL);
        printf("Pool-аллокатор: OK\n");
    }

    printf("\n=== Buddy-аллокатор тест ===\n");
    {
        /* Выделяем буфер с запасом и выравниваем вручную до 1024 байт */
        char raw_buffer[2048];
        uintptr_t addr = (uintptr_t)raw_buffer;
        uintptr_t aligned_addr = (addr + 1023) & ~((uintptr_t)1023);
        char* buffer = (char*)aligned_addr;
        size_t size = 1024;

        assert((char*)raw_buffer + sizeof(raw_buffer) >= buffer + size);
        assert(((uintptr_t)buffer & (size - 1)) == 0); /* Проверка выравнивания */

        BuddyCtx ctx;
        uint8_t orders[BUDDY_ORDERS_COUNT(1024, 16)];
        IAllocator buddy = create_buddy_alloc(&ctx, buffer, size, 16, orders);
        assert(ctx.buffer != NULL); /* Инициализация успешна */

        /* 60 байт -> порядок 6 (64 байта) */
        void* p1 = i_alloc(&buddy, 60);
        /* 30 байт -> порядок 5 (32 байта) */
        void* p2 = i_alloc(&buddy, 30);
        /* 100 байт -> порядок 7 (128 байт) */
        void* p3 = i_alloc(&buddy, 100);
        assert(p1 && p2 && p3);
        printf("Выделено: p1=%p (60->64B), p2=%p (30->32B), p3=%p (100->128B)\n", p1, p2, p3);

        /* Освобождаем p2 (32B) и p1 (64B).
         * p2 сольётся со своим buddy в 64B-блок, но с p1 напрямую не сольётся,
         * т.к. они разного порядка. */
        i_free(&buddy, p2);
        i_free(&buddy, p1);
        void* p4 = i_alloc(&buddy, 50); /* Подберёт подходящий свободный блок */
        assert(p4 != NULL);

        /* Тест realloc: расширяем 128B до 200B -> нужен 256B-блок */
        void* p5 = i_realloc(&buddy, p3, 200);
        assert(p5 != NULL);

        /* Полный сброс и проверка слияния (coalesce) */
        i_reset(&buddy);
        void* half1 = i_alloc(&buddy, 512); /* Порядок 9 */
        void* half2 = i_alloc(&buddy, 512); /* Порядок 9 */
        assert(half1 && half2);
        assert(half1 == buffer);          /* Первый половинный блок — в начале */
        assert(half2 == buffer + 512);    /* Второй — посередине */

        void* fail = i_alloc(&buddy, 1);  /* Всё занято */
        assert(fail == NULL);

        /* Освобождаем обе половины — они должны слиться обратно в 1024B */
        i_free(&buddy, half1);
        i_free(&buddy, half2);
        void* full = i_alloc(&buddy, 1024); /* Занимаем весь буфер целиком */
        assert(full == buffer);

        printf("Buddy-аллокатор: OK (слияние проверено)\n");
    }

    printf("\n=== Все тесты пройдены! ===\n");
    return 0;
}