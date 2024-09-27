#include "generic.h"
#include "prelude.h"
#include "memory.h"

/* Arg check */
#if !defined(container_name) || !defined(container_type_param)
#error "Missing container_name and/or container_type_param"
#endif
#if !defined(container_prefix)
#define container_prefix M_EXP(container_name)
#endif
#define container_func(name) M_GLUE(container_prefix, _##name)

/* Private macros */
#define Dyn_Array M_EXP(container_name)
#define T         M_EXP(container_type_param)

/* Implementation */
typedef struct Dyn_Array Dyn_Array;

struct Dyn_Array {
	T* data;
	isize len;
	isize cap;
	Mem_Allocator allocator;
};

bool container_func(init)(Dyn_Array* arr, isize initial_cap, Mem_Allocator allocator){
	T* data = New(T, initial_cap, allocator);
	if(!data){ return 0; }
	arr->allocator = allocator;
	arr->data = data;
	arr->cap = initial_cap;
	arr->len = 0;

	return 1;
}

void container_func(destroy)(Dyn_Array* arr){
	mem_free(arr->allocator, arr->data);
	arr->data = NULL;
}

/* Private macros */
#undef Dyn_Array
#undef T
#undef container_name
#undef container_type_param
#undef container_prefix
#undef container_func
