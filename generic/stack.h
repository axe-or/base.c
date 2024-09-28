#include "generic.h"
#include "prelude.h"

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
#define Stack M_EXP(container_name)
#define T     M_EXP(container_type_param)

/* Implementation */
struct Stack {
	T* data;
	isize len;
	isize cap;
};

generic_func
void container_func(init)(Stack* stk, T* buf, isize buflen){
	stk->len = 0;
	stk->data = buf;
	stk->cap = buflen;
}

generic_func
bool container_func(push)(Stack* stk, T val){
	if(stk->len >= stk->cap){
		return false;
	}
	stk->data[stk->len] = val;
	stk->len += 1;
	return true;
}

generic_func
T container_func(pop)(Stack* stk){
	if(stk->len <= 0){
		return false;
	}
	stk->len -= 1;
	return stk->data[stk->len];
}

generic_func
bool container_func(pop_into)(Stack* stk, T* into){
	if(stk->len <= 0){
		return false;
	}
	stk->len -= 1;
	*into = stk->data[stk->len];
	return true;
}

#undef Stack
#undef T
#undef container_name
#undef container_type_param
#undef container_prefix
#undef container_func
#undef generic_func
