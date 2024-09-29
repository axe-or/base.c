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
	ia_append(&arr, 69);
	ia_append(&arr, 420);
	ia_append(&arr, -200);
	printf("len: %ld cap: %ld data: { ", arr.len, arr.cap); for(isize i = 0; i < arr.len; i ++){ printf("%ld ", arr.data[i]); } printf("}\n");
}
