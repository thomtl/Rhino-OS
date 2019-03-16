#include <rhino/types/linked_list.h>

void linked_list_destroy(linked_list_t * list){
    linked_list_node_t* n = list->head;
    while(n){
        kfree(n->data);
        n = n->next;
    }
}

void linked_list_free(linked_list_t * list){
    linked_list_node_t* n = list->head;
    while(n){
        linked_list_node_t* next = n->next;
        kfree(n);
        n = next;
    }
}

void linked_list_append(linked_list_t * list, linked_list_node_t * item){
    ASSERT(!(item->next || item->previous));

    item->next = NULL;

    item->owner = list;
    if(list->length == 0){
        list->head = item;
        list->tail = item;
        item->previous = NULL;
        item->next = NULL;
        list->length++;
        return;
    }

    list->tail->next = item;
    item->previous = list->tail;
    list->tail = item;
    list->length++;
}
linked_list_node_t* linked_list_insert(linked_list_t * list, void * item){
    linked_list_node_t* node = kmalloc(sizeof(linked_list_node_t));
    node->data = item;
    node->next = NULL;
    node->previous= NULL;
    node->owner = NULL;
    linked_list_append(list, node);
    return node;
}
linked_list_t* linked_list_create(void){
    linked_list_t* out = kmalloc(sizeof(linked_list_t));
    out->head = NULL;
    out->tail = NULL;
    out->length = 0;
    return out;
}

linked_list_node_t* linked_list_find(linked_list_t * list, void * value){
    for(linked_list_node_t* i = (list)->head; i != NULL; i = i->next){
        if(i->data == value) return i;
    }
    return NULL;
}

int linked_list_index_of(linked_list_t * list, void * value){
    uint32_t index = 0;
    for(linked_list_node_t* i = (list)->head; i != NULL; i = i->next){
        if(i->data == value) return index;
        index++;
    }
    return -1;
}

void linked_list_remove(linked_list_t * list, size_t index){
    if(index > list->length) return;
    uint32_t i = 0;
    linked_list_node_t* n = list->head;
    while(i < index){
        n = n->next;
        i++;
    }
    linked_list_delete(list, n);
}

void linked_list_delete(linked_list_t * list, linked_list_node_t * node){
    ASSERT(node->owner == list);

    if(node == list->head) list->head = node->next;
    if(node == list->tail) list->tail = node->previous;
    if(node->previous) node->previous->next = node->next;
    if(node->next) node->next->previous = node->previous;
    node->previous = NULL;
    node->next = NULL;
    node->owner = NULL;
    list->length--;
}

linked_list_node_t* linked_list_pop(linked_list_t * list){
    if(!list->tail) return NULL;
    linked_list_node_t* out = list->tail;
    linked_list_delete(list, out);
    return out;
}
linked_list_node_t* linked_list_dequeue(linked_list_t * list){
    if(!list->head) return NULL;
    linked_list_node_t* out = list->head;
    linked_list_delete(list, out);
    return out;
}
linked_list_t* linked_list_copy(linked_list_t * original){
    linked_list_t* out = linked_list_create();
    linked_list_node_t* node = original->head;
    while(node) linked_list_insert(out, node->data);
    return out;
}
void linked_list_merge(linked_list_t * target, linked_list_t * source){
    for(linked_list_node_t* i = (source)->head; i != NULL; i = i->next){
        i->owner = target;
    }

    if(source->head) source->head->previous = target->tail;
    if(target->tail) target->tail->next = source->head;
    else target->head = source->head;
    if(source->tail) target->tail = source->tail;

    target->length += source->length;
    kfree(source);
}

void linked_list_append_after(linked_list_t * list, linked_list_node_t * before, linked_list_node_t * node){
    ASSERT(!(node->next || node->previous));

    node->owner = list;

    if(!list->length){
        linked_list_append(list, node);
        return;
    }
    if(before == NULL){
        node->next = list->head;
        node->previous = NULL;
        list->head->previous = node;
        list->head = node;
        list->length++;
        return;
    }
    if(before == list->tail) list->tail = node;
    else{
        before->next->previous = node;
        node->next = before->next;
    }

    node->previous = before;
    before->next = node;
    list->length++;
}
linked_list_node_t* linked_list_insert_after(linked_list_t* list, linked_list_node_t* before, void* item){
    linked_list_node_t* node = kmalloc(sizeof(linked_list_node_t));
    node->data = item;
    node->next = NULL;
    node->previous = NULL;
    node->owner = NULL;
    linked_list_append_after(list, before, node);
    return node;
}

void linked_list_append_before(linked_list_t * list, linked_list_node_t * after, linked_list_node_t* node){
    ASSERT(!(node->next || node->previous));

    node->owner = list;
    if(!list->length){
        linked_list_append(list, node);
        return;
    }

    if(after == NULL){
        node->next = NULL;
        node->previous = list->tail;
        list->tail->next = node;
        list->tail = node;
        list->length++;
        return;
    }
    if(after == list->head){
        list->head = node;
    } else {
        after->previous->next = node;
        node->previous = after->previous;
    }
    node->next = after;
    after->previous = node;
    list->length++;
}

linked_list_node_t* linked_list_insert_before(linked_list_t* list, linked_list_node_t* after, void* item){
    linked_list_node_t* node = kmalloc(sizeof(linked_list_node_t));
    node->data = item;
    node->next = NULL;
    node->previous = NULL;
    node->owner = NULL;
    linked_list_append_before(list, after, node);
    return node;
}