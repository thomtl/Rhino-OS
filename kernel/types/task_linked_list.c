#include "task_linked_list.h"
#include "../heap/kheap.h"
#include "../../libc/include/string.h"

static task_linked_list_entry_t* findLastEntry(task_linked_list_t* list){
  task_linked_list_entry_t* currentEntry = list->start;
  while(currentEntry != NULL){
    currentEntry = currentEntry->next;
  }
  return currentEntry;
}

static task_linked_list_entry_t* findEntryWithIndex(task_linked_list_t* list, uint32_t n){
  task_linked_list_entry_t* currentEntry = list->start;
  for(uint32_t i = 0; i < n; i++){
    currentEntry = currentEntry->next;
  }
  return currentEntry;
}

task_linked_list_t* createLinkedList(task_t rootEntry){
  task_linked_list_entry_t* entryAddress = kmalloc(sizeof(task_linked_list_entry_t));
  memset(entryAddress, 0, sizeof(task_linked_list_entry_t));
  task_linked_list_entry_t entry;
  entry.task = rootEntry;
  entry.next = NULL;
  *entryAddress = entry;
  task_linked_list_t* list = kmalloc(sizeof(task_linked_list_t));
  memset(entryAddress, 0, sizeof(task_linked_list_t));
  list->start = entryAddress;
  return list;
}
void insertEntry(task_linked_list_t* list, task_t task){
  task_linked_list_entry_t* previousEntry = findLastEntry(list);
  task_linked_list_entry_t* entryAddress = kmalloc(sizeof(task_linked_list_entry_t));
  memset(entryAddress, 0, sizeof(task_linked_list_entry_t));
  task_linked_list_entry_t entry;
  entry.task = task;
  entry.next = NULL;
  *entryAddress = entry;
  previousEntry->next = entryAddress;
}
void deleteEntry(task_linked_list_t* list, uint32_t n){
  task_linked_list_entry_t *entry = findEntryWithIndex(list, n);
  task_linked_list_entry_t *previousEntry = findEntryWithIndex(list, n - 1);
  task_linked_list_entry_t *nextEntry = findEntryWithIndex(list, n + 1);
  previousEntry->next = nextEntry;
  kfree(entry);
}
task_linked_list_entry_t* findEntry(task_linked_list_t* list, uint32_t n){
  return findEntryWithIndex(list, n);
}
