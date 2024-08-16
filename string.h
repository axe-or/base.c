#pragma once

#include "prelude.h"
#include "mem.h"

// Helper to use with printf
#define FMT_STRING(str_) (int)((str_).len), (str_).data

typedef struct {
	isize len;
	byte const * data;
} String;

static inline
isize cstring_len(cstring cstr){
	static const isize CSTR_MAX_LENGTH = (~(u32)0) >> 1;
	isize size = 0;
	for(isize i = 0; i < CSTR_MAX_LENGTH && cstr[i] != 0; i += 1){
		size += 1;
	}
	return size;
}

String str_from(cstring data);

String str_from_range(cstring data, isize start, isize length);

String str_from_bytes(byte const* data, isize length);

String str_sub(String s, isize start, isize length);

String str_clone(String s, Mem_Allocator allocator);

void str_destroy(String s, Mem_Allocator allocator);

String str_concat(String a, String b, Mem_Allocator allocator);

bool str_eq(String a, String b);


