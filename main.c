#define BASE_C_IMPLEMENTATION 1
#include "prelude.h"
#include "heap_allocator.h"
#include <stdio.h>

#define container_name Int_Vector
#define container_prefix ivec
#define container_type_param i64
#include "generic/dynamic_array.h"

int main(){
	Int_Vector arr;
	ivec_init(&arr, 16, heap_allocator());
}
