#include <rhino/types/binary_tree.h>

binary_tree_t* binary_tree_create(){
    binary_tree_t* out = kmalloc(sizeof(binary_tree_t));
    out->nodes = 0;
    out->root = NULL;
    return out;
}

void binary_tree_set_root(binary_tree_t * tree, void * value){
    binary_tree_node_t* root = binary_tree_node_create(value);
    tree->root = root;
    tree->nodes = 1;
}

void binary_tree_node_destroy(binary_tree_node_t * node){
    linked_list_t* list = node->children;
    for(linked_list_node_t* item = list->head; item != NULL; item = item->next){
        binary_tree_node_destroy((binary_tree_node_t*)item->data);
    }
    kfree(node->value);
}

void binary_tree_destroy(binary_tree_t * tree){
    if(tree->root) binary_tree_node_destroy(tree->root);
}


void binary_tree_node_free(binary_tree_node_t * node){
    if(!node) return;
    linked_list_t* list = node->children;
    for(linked_list_node_t* item = list->head; item != NULL; item = item->next){
        binary_tree_node_free((binary_tree_node_t*)item->data);
    }
    kfree(node);
}

void binary_tree_free(binary_tree_t * tree){
    binary_tree_node_free(tree->root);
}

binary_tree_node_t * binary_tree_node_create(void * value){
    binary_tree_node_t* out = kmalloc(sizeof(binary_tree_node_t));
    out->value = value;
    out->children = linked_list_create();
    out->parent = NULL;
    return out;
}

void binary_tree_node_insert_child_node(binary_tree_t * tree, binary_tree_node_t * parent, binary_tree_node_t * node){
    linked_list_insert(parent->children, node);
    node->parent = parent;
    tree->nodes++;
}

binary_tree_node_t * binary_tree_node_insert_child(binary_tree_t * tree, binary_tree_node_t * parent, void * value){
    binary_tree_node_t* out =  binary_tree_node_create(value);
    binary_tree_node_insert_child_node(tree, parent, out);
    return out;
}

binary_tree_node_t * binary_tree_node_find_parent(binary_tree_node_t * haystack, binary_tree_node_t * needle){
    binary_tree_node_t* found = NULL;
    for(linked_list_node_t *child = haystack->children->head; child != NULL; child = child->next){
        if(child->data == needle) return haystack;
        found = binary_tree_node_find_parent((binary_tree_node_t*)child->data, needle);
        if(found) break;
    }
    return found;
}

binary_tree_node_t * tree_find_parent(binary_tree_t * tree, binary_tree_node_t * node){
    
    if(!tree->root) return NULL;
    return binary_tree_node_find_parent(tree->root, node);
}

size_t binary_tree_count_children(binary_tree_node_t* node){
    if(!node) return 0;
    if(!node->children) return 0;
    size_t out = node->children->length;
    for (linked_list_node_t * i = (node->children)->head; i != NULL; i = i->next){
        out += binary_tree_count_children((binary_tree_node_t*)i->data);
    }
    return out;
}

void binary_tree_node_parent_remove(binary_tree_t * tree, binary_tree_node_t * parent, binary_tree_node_t * node){
    tree->nodes -= binary_tree_count_children(node) + 1;
    linked_list_delete(parent->children, linked_list_find(parent->children, node));
    binary_tree_node_free(node);
}

void binary_tree_node_remove(binary_tree_t * tree, binary_tree_node_t * node){
    binary_tree_node_t* parent = node->parent;
    if(!parent){
        if(node == tree->root){
            tree->nodes = 0;
            tree->root = NULL;
            binary_tree_node_free(node);
        }
    }
    binary_tree_node_parent_remove(tree, parent, node);
}

void binary_tree_remove(binary_tree_t * tree, binary_tree_node_t * node){
    binary_tree_node_t* parent = node->parent;

    if(!parent) return;
    tree->nodes--;
    linked_list_delete(parent->children, linked_list_find(parent->children, node));
    for(linked_list_node_t* i = node->children->head; i != NULL; i = i->next){
        ((binary_tree_node_t*)i->data)->parent = parent;
    }
    linked_list_merge(parent->children, node->children);
    kfree(node);
}

void binary_tree_reparent_root(binary_tree_t* tree, binary_tree_node_t* node){
    binary_tree_node_t* parent = node->parent;
    if(!parent) return;
    tree->nodes--;
    linked_list_delete(parent->children, linked_list_find(parent->children, node));
    for(linked_list_node_t* i = node->children->head; i != NULL; i = i->next){
        ((binary_tree_node_t*)i->data)->parent = tree->root;
    }
    linked_list_merge(tree->root->children, node->children);
    kfree(node);
}

binary_tree_node_t* binary_tree_find_node(binary_tree_node_t* node, void* search, binary_tree_comparator_t comparator){
    if(comparator(node->value, search)) return node;
    binary_tree_node_t* found;
    for(linked_list_node_t* i = node->children->head; i != NULL; i = i->next){
        found = binary_tree_find_node((binary_tree_node_t*)i->data, search, comparator);
        if(found) return found;
    }
    return NULL;
}

binary_tree_node_t * binary_tree_find(binary_tree_t * tree, void * value, binary_tree_comparator_t comparator){
    return binary_tree_find_node(tree->root, value, comparator);
}

void binary_tree_break_off(binary_tree_t * tree, binary_tree_node_t * node){
    binary_tree_node_t* parent = node->parent;
    if(!parent) return;
    linked_list_delete(parent->children, linked_list_find(parent->children, node));
    UNUSED(tree);
}