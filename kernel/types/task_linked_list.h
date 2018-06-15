#ifndef TASK_LINKED_LIST_H
#define TASK_LINKED_LIST_H
#include <stdint.h>
#include "../multitasking/task.h"

typedef struct task_linked_list_entry {
  task_t *task;
  struct task_linked_list_entry *next;
} task_linked_list_entry_t;

typedef struct {
  task_linked_list_entry_t* start;
} task_linked_list_t;

task_linked_list_t* createLinkedList(task_t *rootEntry);
void insertEntry(task_linked_list_t* list, task_t *task);
void deleteEntry(task_linked_list_t* list, uint32_t n);
task_linked_list_entry_t* findEntry(task_linked_list_t* list, uint32_t n);
task_t* getLastEntry(task_linked_list_t *list);
uint32_t getIndex(task_linked_list_t *list, task_t* task);
#endif
