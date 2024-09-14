#pragma once

#include "prelude.h"

// Single linked list

// Double linked list
typedef struct DList_Part DList_Part;

struct DList_Part {
	DList_Part* prev;
	DList_Part* next;
};

void dlist_insert_after(DList_Part* target, DList_Part* current){
	if(target == NULL){ return; }
	DList_Part* old = target->next;
	target->next = current;
	current->prev = target;
	current->next = old;
	if(current->next->next){
		current->next->next = current;
	}
}

void dlist_insert_before(DList_Part* target, DList_Part* current){
	if(target == NULL){ return; }
	DList_Part* old = target->prev;
	target->prev = current;
	current->next = target;
	current->prev = old;
	if(current->prev->prev){
		current->prev->prev = current;
	}
}

#ifdef BASE_C_IMPLEMENTATION
#endif
