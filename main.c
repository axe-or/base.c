#define BASE_C_IMPLEMENTATION 1
#include "prelude.h"
#include "heap_allocator.h"
#include <stdio.h>

#define container_name Int_Array
#define container_type_param i64
#define container_prefix ia
#include "generic/dynamic_array.h"

int main(){
	Int_Array arr;
	if(!ia_init(&arr, heap_allocator(), 1)){
		panic("Failed to init array");
	}
	printf("len: %ld cap: %ld data: { ", arr.len, arr.cap); for(isize i = 0; i < arr.len; i ++){ printf("%ld ", arr.data[i]); } printf("}\n");
	for(int i = 0; i < 20; i ++) ia_append(&arr, i);
	printf("len: %ld cap: %ld data: { ", arr.len, arr.cap); for(isize i = 0; i < arr.len; i ++){ printf("%ld ", arr.data[i]); } printf("}\n");
	ia_shrink(&arr);
	printf("len: %ld cap: %ld data: { ", arr.len, arr.cap); for(isize i = 0; i < arr.len; i ++){ printf("%ld ", arr.data[i]); } printf("}\n");
	ia_remove(&arr, arr.len - 1);
	ia_remove(&arr, 16);
	ia_remove(&arr, 0);
	printf("len: %ld cap: %ld data: { ", arr.len, arr.cap); for(isize i = 0; i < arr.len; i ++){ printf("%ld ", arr.data[i]); } printf("}\n");
	ia_insert(&arr, 0, -420);
	ia_insert(&arr, 16, -420);
	ia_insert(&arr, arr.len, -420);
	printf("len: %ld cap: %ld data: { ", arr.len, arr.cap); for(isize i = 0; i < arr.len; i ++){ printf("%ld ", arr.data[i]); } printf("}\n");
	ia_resize(&arr, 8);
	ia_pop(&arr);
	ia_resize(&arr, 10);
	printf("len: %ld cap: %ld data: { ", arr.len, arr.cap); for(isize i = 0; i < arr.len; i ++){ printf("%ld ", arr.data[i]); } printf("}\n");
	ia_destroy(&arr);
}
