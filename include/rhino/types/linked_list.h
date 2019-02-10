#pragma once

#include <rhino/common.h>
#include <rhino/mm/hmm.h>

struct linked_list_node {
    void* data;
    void* owner;
    struct linked_list_node* next;
    struct linked_list_node* previous;
};

typedef struct linked_list_node linked_list_node_t;

typedef struct {
    linked_list_node_t* head;
    linked_list_node_t* tail;
    uint32_t length;
} linked_list_t;

void linked_list_destroy(linked_list_t * list);
void linked_list_free(linked_list_t * list);
void linked_list_append(linked_list_t * list, linked_list_node_t * item);
linked_list_node_t* linked_list_insert(linked_list_t * list, void * item);
linked_list_t* linked_list_create(void);
linked_list_node_t* linked_list_find(linked_list_t * list, void * value);
int linked_list_index_of(linked_list_t * list, void * value);
void linked_list_remove(linked_list_t * list, size_t index);
void linked_list_delete(linked_list_t * list, linked_list_node_t * node);
linked_list_node_t* linked_list_pop(linked_list_t * list);
linked_list_node_t* linked_list_dequeue(linked_list_t * list);
linked_list_t* linked_list_copy(linked_list_t * original);
void linked_list_merge(linked_list_t * target, linked_list_t * source);

void linked_list_append_after(linked_list_t * list, linked_list_node_t * before, linked_list_node_t * node);
linked_list_node_t* linked_list_insert_after(linked_list_t* list, linked_list_node_t* before, void* item);

void linked_list_append_before(linked_list_t * list, linked_list_node_t * after, linked_list_node_t* node);
linked_list_node_t* linked_list_insert_before(linked_list_t* list, linked_list_node_t* after, void* item);