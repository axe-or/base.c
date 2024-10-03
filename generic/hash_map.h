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
typedef struct container_name container_name;
#define generic_func static inline

/* Private macros */
#define Hash_Map M_EXP(container_name)
#define KT       M_EXP(container_type_param_key)
#define VT       M_EXP(container_type_param_val)

struct Hash_Map {
	VT* slots;
	KT* keys;
	isize cap;
	isize count;
	isize (*hash_func)(void* p, isize n);
	Mem_Allocator allocator;
};

generic_func
bool container_func(init)(Hash_Map* map, Mem_Allocator allocator, isize cap){
}

#undef Hash_Map
#undef T
#undef container_name
#undef container_type_param_key
#undef container_type_param_val
#undef container_prefix
#undef container_func
#undef generic_func
