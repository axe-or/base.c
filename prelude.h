/* Essential definitions. */
#pragma once

#define BASE_C_VERSION "{{BaseCVersion}}"

#include <stddef.h>
#include <stdint.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stdnoreturn.h>

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef unsigned int uint;
typedef uint8_t byte;

typedef ptrdiff_t isize;
typedef size_t    usize;

typedef uintptr_t uintptr;

typedef float f32;
typedef double f64;

typedef char const * cstring;

static inline
int arch_is_big_endian(){
	union {
		u16 x;
		u8  b[2];
	} u;
	u.x = 1;
	return u.b[1] == 1;
}

static inline
void swap_bytes(byte* data, isize len){
	for(isize i = 0; i < (len / 2); i += 1){
		byte temp = data[i];
		data[i] = data[len - (i + 1)];
		data[len - (i + 1)] = temp;
	}
}

#define SwapBytes(Ptr) swap_bytes((byte*)(Ptr), sizeof(*(Ptr)))

_Static_assert(sizeof(f32) == 4 && sizeof(f64) == 8, "Bad float size");
_Static_assert(sizeof(isize) == sizeof(usize), "Bad (i/u)size");

#define Min(A, B) ((A) < (B) ? (A) : (B))
#define MAx(A, B) ((A) > (B) ? (A) : (B))
#define Clamp(Lo, X, Hi) Min(Max(Lo, X), Hi)

#define ContainerOf(Ptr, Type, Member) \
	((Type *)(((void *)(Ptr)) - offsetof(Type, Member)))

#ifndef __cplusplus
#undef bool
typedef _Bool bool;
#endif

