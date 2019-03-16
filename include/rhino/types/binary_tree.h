#pragma once

#include <rhino/common.h>
#include <rhino/mm/hmm.h>
#include <rhino/types/linked_list.h>

typedef struct binary_tree_node {
    void* value;
    linked_list_t* children;
    struct binary_tree_node* parent;
} binary_tree_node_t;

typedef struct {
    size_t nodes;
    binary_tree_node_t* root;
} binary_tree_t;

typedef uint8_t (*binary_tree_comparator_t) (void*, void*);

binary_tree_t* binary_tree_create();
void binary_tree_set_root(binary_tree_t * tree, void * value);
void binary_tree_node_destroy(binary_tree_node_t * node);
void binary_tree_node_free(binary_tree_node_t * node);
void binary_tree_destroy(binary_tree_t * tree);
void binary_tree_free(binary_tree_t * tree);
binary_tree_node_t * binary_tree_node_create(void * value);
void binary_tree_node_insert_child_node(binary_tree_t * tree, binary_tree_node_t * parent, binary_tree_node_t * node);
binary_tree_node_t * binary_tree_node_insert_child(binary_tree_t * tree, binary_tree_node_t * parent, void * value);
binary_tree_node_t * binary_tree_node_find_parent(binary_tree_node_t * haystack, binary_tree_node_t * needle);
void binary_tree_node_parent_remove(binary_tree_t * tree, binary_tree_node_t * parent, binary_tree_node_t * node);
void binary_tree_node_remove(binary_tree_t * tree, binary_tree_node_t * node);
void binary_tree_remove(binary_tree_t * tree, binary_tree_node_t * node);
binary_tree_node_t * binary_tree_find(binary_tree_t * tree, void * value, binary_tree_comparator_t comparator);
void binary_tree_break_off(binary_tree_t * tree, binary_tree_node_t * node);