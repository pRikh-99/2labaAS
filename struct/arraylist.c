#include "arraylist.h"
#include <string.h>

void arraylist_init(ArrayList* list, IAllocator* alloc) {
    list->alloc = alloc;
    list->capacity = 4;
    list->data = i_alloc(alloc, list->capacity * sizeof(void*));
    list->size = 0;
}

void arraylist_add(ArrayList* list, void* value) {
    if (list->size == list->capacity) {
        list->capacity *= 2;
        void** new_data = i_alloc(list->alloc, list->capacity * sizeof(void*));
        memcpy(new_data, list->data, list->size * sizeof(void*));
        i_free(list->alloc, list->data);
        list->data = new_data;
    }
    list->data[list->size] = value;
    list->size++;
}

void* arraylist_get(ArrayList* list, size_t index) {
    if (index < list->size) return list->data[index];
    return NULL;
}

void arraylist_remove(ArrayList* list, size_t index) {
    if (index >= list->size) return;
    for (size_t i = index; i < list->size - 1; i++) {
        list->data[i] = list->data[i + 1];
    }
    list->size--;
}

void arraylist_destroy(ArrayList* list) {
    i_free(list->alloc, list->data);
}