CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -I.

# Исходники аллокаторов
SRCS = allocator.c sys_alloc.c linear_allocator.c pool_allocator.c buddy_allocator.c
OBJS = $(SRCS:.c=.o)

# Исходники структур
STRUCT_SRCS = struct/arraylist.c struct/queue.c struct/hashtable.c
STRUCT_OBJS = $(STRUCT_SRCS:.c=.o)

# Заголовки
HDRS = allocator.h sys_alloc.h linear_allocator.h pool_allocator.h buddy_allocator.h
STRUCT_HDRS = struct/arraylist.h struct/queue.h struct/hashtable.h

.PHONY: all clean test test_struct test_run

all: test

# Аллокаторы
%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

# Структуры
struct/%.o: struct/%.c $(STRUCT_HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

# Тест аллокаторов
test: $(OBJS) test.c $(HDRS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) test.c

# Тест структур
test_struct: $(OBJS) $(STRUCT_OBJS) test_struct.c
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(STRUCT_OBJS) test_struct.c

# Запуск
test_run: test test_struct
	./test
	./test_struct

clean:
	rm -rf $(OBJS) $(STRUCT_OBJS) test test_struct