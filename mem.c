#include "mem.h"

i32 allocator_query_capabilites(Mem_Allocator allocator, i32* capabilities){
	if(capabilities == NULL){ return 0; }
	allocator.func(allocator.data, Mem_Op_Query, NULL, 0, 0, capabilities);
	return *capabilities;
}

void* mem_alloc(Mem_Allocator allocator, isize size, isize align){
	void* ptr = allocator.func(allocator.data, Mem_Op_Alloc, NULL, size, align, NULL);
	if(ptr != NULL){
		mem_set(ptr, 0, size);
	}
	return ptr;
}

void* mem_resize(Mem_Allocator allocator, void* ptr, isize new_size){
	void* new_ptr = allocator.func(allocator.data, Mem_Op_Resize, ptr, new_size, 0, NULL);
	return new_ptr;
}

void mem_free(Mem_Allocator allocator, void* p){
	if(p == NULL){ return; }
	allocator.func(allocator.data, Mem_Op_Free, p, 0, 0, NULL);
}

void mem_free_all(Mem_Allocator allocator){
	allocator.func(allocator.data, Mem_Op_Free_All, NULL, 0, 0, NULL);
}

