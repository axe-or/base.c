#include "./list.h"

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