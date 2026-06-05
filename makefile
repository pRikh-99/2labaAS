CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11

SRCS = allocator.c sys_alloc.c linear_allocator.c pool_allocator.c buddy_allocator.c
OBJS = $(SRCS:.c=.o)

HDRS = allocator.h sys_alloc.h linear_allocator.h pool_allocator.h buddy_allocator.h

.PHONY: all clean test test_run

all: test

%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

test: $(OBJS) test.c $(HDRS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) test.c
	chmod +x $@

test_run: test
	./test

clean:
	rm -rf $(OBJS) test