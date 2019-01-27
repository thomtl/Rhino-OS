#pragma once

#include <rhino/common.h>
#include <rhino/mm/hmm.h>

struct linked_list_node {
    void* data;
    uint32_t data_size;
    uint32_t user_flags;
    uint32_t key;
    struct linked_list* next;
    struct linked_list* previous;
};

typedef struct linked_list_node linked_list_node_t;

typedef struct {
    linked_list_node_t* head;
    linked_list_node_t* current;
} linked_list_t;

void linked_list_create(linked_list_t* list);
void linked_list_destruct(linked_list_t* list);
void linked_list_push_back(linked_list_t* list, void* data, uint32_t data_size, uint32_t key, uint32_t user_flags);
void linked_list_insert_first(linked_list_t* list, void* data, uint32_t data_size, uint32_t key, uint32_t user_flags);
void linked_list_delete_first(linked_list_t* list);
bool linked_list_is_empty(linked_list_t* list);
void linked_list_delete(linked_list_t *list, uint32_t key);
linked_list_node_t* linked_list_find(linked_list_t* list, uint32_t key);

