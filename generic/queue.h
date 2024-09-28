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
#define Queue M_EXP(container_name)
#define T     M_EXP(container_type_param)

struct Queue {
	T* data;
	isize cap;
	isize len;
	isize base;
};

generic_func
void container_func(init)(Queue* q, T* data, isize cap){
	q->data = data;
	q->cap = cap;
	q->len = 0;
	q->base = 0;
}

generic_func
bool container_func(push)(Queue* q, T val){
	if(q->len >= q->cap){
		return false;
	}
	isize pos = (q->base + q->len) % q->cap;
	q->data[pos] = val;
	q->len += 1;
	return true;
}

generic_func
T container_func(pop)(Queue* q){
	if(q->len <= 0){
		return (T){0};
	}
	T val = q->data[q->base];
	q->len -= 1;
	q->base = (q->base + 1) % q->cap;
	return true;
}

generic_func
bool container_func(pop_into)(Queue* q, T* into){
	if(q->len <= 0){
		return false;
	}
	*into = container_func(pop)(q);
	return true;
}


#undef Queue
#undef T
#undef container_name
#undef container_type_param
#undef container_prefix
#undef container_func
#undef generic_func
