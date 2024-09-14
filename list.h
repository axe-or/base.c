/* Intrusive circular doubly linked list. This is similar to the `list_head`
 * construct used in the Linux kernel, although very slimmed down for the sake
 * of simplicity. */
#pragma once

#include "prelude.h"

typedef struct List_Node List_Node;

struct List_Node {
	List_Node* next;
	List_Node* prev;
};

// Add list value after target
void list_add(List_Node* target, List_Node* new_node);

// Delete value from list by redirecting its neighbors
void list_add(List_Node* target, List_Node* new_node);

// Initialize node to be head of list
static inline
void list_init(List_Node* target){
	target->next = target;
	target->prev = target;
}

// Get pointer of structure containing the list
#define list_entry(Ptr, Type, Member) ContainerOf(Ptr, Type, Member)

#define list_foreach(IterVar, ListHead) \
	for(List_Node* IterVar = ListHead.next; IterVar != &ListHead; IterVar = IterVar->next)

#define list_foreach_reversed(IterVar, ListHead) \
	for(List_Node* IterVar = ListHead.prev; IterVar != &ListHead; IterVar = IterVar->prev)

#ifdef BASE_C_IMPLEMENTATION

// Insert new_node between 2 existing nodes
static void _list_add(List_Node* prev, List_Node* next, List_Node* new_node){
	new_node->next = next;
	new_node->prev = prev;
	if(next != NULL){ next->prev = new_node; }
	if(prev != NULL){ prev->next = new_node; }
}

static void _list_del(List_Node* node){
	List_Node* prev = node->prev;
	List_Node* next = node->next;
	if(next != NULL){ next->prev = prev; }
	if(prev != NULL){ next->prev = next; }
}

void list_add(List_Node* target, List_Node* new_node){
	_list_add(target, target->next, new_node);
}

void list_del(List_Node* node){
	_list_del(node);
}

#endif
