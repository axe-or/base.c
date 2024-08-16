#pragma once

#include "prelude.h"
#include "assert.h"

#define New(T_, N_, Al_) mem_alloc((Al_), sizeof(T_) * (N_), alignof(T_))

enum Allocator_Op {
	Mem_Op_Alloc    = 1,
	Mem_Op_Resize   = 2,
	Mem_Op_Free     = 3,
	Mem_Op_Free_All = 4,

	Mem_Op_Query = 0,
};

enum Allocator_Capability {
	Allocator_Alloc_Any = 1 << 0,
	Allocator_Free_Any  = 1 << 1,
	Allocator_Free_All  = 1 << 2,
	Allocator_Resize    = 1 << 3,
};

typedef void* (*Mem_Allocator_Func) (
	void* impl,
	enum Allocator_Op op,
	void* old_ptr,
	isize size, isize align,
	i32* capabilities
);

typedef struct {
	Mem_Allocator_Func func;
	void* data;
} Mem_Allocator;

static inline
void mem_set(void* p, byte val, isize nbytes){
	__builtin_memset(p, val, nbytes);
}

static inline
void mem_copy(void* dest, void const * src, isize nbytes){
	__builtin_memmove(dest, src, nbytes);
}

static inline
int mem_valid_alignment(isize align){
	return (align & (align - 1)) == 0 && (align != 0);
}

// Align p to alignment a, this only works if a is a non-zero power of 2
static inline
uintptr align_forward_ptr(uintptr p, uintptr a){
	debug_assert(mem_valid_alignment(a), "Invalid memory alignment");
	uintptr mod = p & (a - 1);
	if(mod > 0){
		p += (a - mod);
	}
	return p;
}

// Get capabilities of allocator as a number, you can use bit operators to check it.
i32 allocator_query_capabilites(Mem_Allocator allocator, i32* capabilities);

// Allocate fresh memory, filled with 0s. Returns NULL on failure.
void* mem_alloc(Mem_Allocator allocator, isize size, isize align);

// Re-allocate memory in-place without changing the original pointer. Returns NULL on failure.
void* mem_resize(Mem_Allocator allocator, void* ptr, isize new_size);

// Free pointer of memory, freeing NULL is a no-op
void mem_free(Mem_Allocator allocator, void* p);

// Free all pointers owned by allocator
void mem_free_all(Mem_Allocator allocator);
