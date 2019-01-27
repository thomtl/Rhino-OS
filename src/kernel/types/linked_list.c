#include <rhino/types/linked_list.h>

void linked_list_create(linked_list_t* list){
    list->head = NULL;
    list->current = NULL;
}

void linked_list_destruct(linked_list_t* list){
    linked_list_node_t* current = list->head;
    while(current != NULL){
        linked_list_node_t* next = current->next;
        kfree((void*)current);
        current = next;
    }

    linked_list_create(list);
}
void linked_list_push_back(linked_list_t* list, void* data, uint32_t data_size, uint32_t key, uint32_t user_flags){
    linked_list_node_t* current = list->head, *link = (linked_list_node_t*)kmalloc(sizeof(linked_list_node_t));

    if(list->head == NULL){
        kfree((void*)link);
        return;
    }

    while(current->next != NULL) current = current->next;

    link->key = key;
    link->data = data;
    link->data_size = data_size;
    link->user_flags = user_flags;

    current->next = link;
    link->next = NULL;

    return;
}

void linked_list_insert_first(linked_list_t* list, void* data, uint32_t data_size, uint32_t key, uint32_t user_flags){
    linked_list_node_t* link = (linked_list_node_t*)kmalloc(sizeof(linked_list_node_t));

    link->key = key;
    link->data = data;
    link->data_size = data_size;
    link->user_flags = user_flags;

    link->next = list->head;

    list->head = link;
}
void linked_list_delete_first(linked_list_t* list){
    list->head = list->head->next;
}

bool linked_list_is_empty(linked_list_t* list){
    return (list->head == NULL);
}

void linked_list_delete(linked_list_t *list, uint32_t key){
    linked_list_node_t* current = list->head, *previous = NULL;

    if(list->head == NULL) return;

    while(current->key != key){
        if(current->next == NULL) return;
        else{
            previous = current;
            current = current->next;
        }
    }

    if(current == list->head){
        list->head = list->head->next;
    } else {
        previous->next = current->next;
    }

    kfree(current);
    return;
}

linked_list_node_t* linked_list_find(linked_list_t* list, uint32_t key){
    linked_list_node_t* current = list->head;

    if(list->head == NULL) return NULL;

    while(current->key != key){
        if(current->next == NULL) return NULL;
        else current = current->next;
    }

    return current;
}

