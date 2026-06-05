#include "arraylist.h"
#include <string.h>

void arraylist_init(ArrayList* list, IAllocator* alloc, size_t initial_capacity) {
    list->alloc = alloc;
    list->size = 0;
    list->capacity = initial_capacity > 0 ? initial_capacity : 4;
    list->data = (void**)i_alloc(alloc, list->capacity * sizeof(void*));
}

void arraylist_destroy(ArrayList* list) {
    if (list->data) {
        i_free(list->alloc, list->data);
        list->data = NULL;
    }
    list->size = 0;
    list->capacity = 0;
}

static int arraylist_grow(ArrayList* list) {
    size_t new_capacity = list->capacity * 2;
    void** new_data = (void**)i_realloc(list->alloc, list->data, new_capacity * sizeof(void*));

    if (!new_data) {
        new_data = (void**)i_alloc(list->alloc, new_capacity * sizeof(void*));
        if (!new_data) return 0;

        memcpy(new_data, list->data, list->size * sizeof(void*));
        i_free(list->alloc, list->data);
    }

    list->data = new_data;
    list->capacity = new_capacity;
    return 1;
}

int arraylist_push(ArrayList* list, void* item) {
    if (list->size >= list->capacity) {
        if (!arraylist_grow(list)) return 0;
    }
    list->data[list->size++] = item;
    return 1;
}

void* arraylist_pop(ArrayList* list) {
    if (list->size == 0) return NULL;
    return list->data[--list->size];
}

void* arraylist_get(ArrayList* list, size_t index) {
    if (index >= list->size) return NULL;
    return list->data[index];
}

int arraylist_set(ArrayList* list, size_t index, void* item) {
    if (index >= list->size) return 0;
    list->data[index] = item;
    return 1;
}

void arraylist_clear(ArrayList* list) {
    list->size = 0;
}

size_t arraylist_size(const ArrayList* list) {
    return list->size;
}