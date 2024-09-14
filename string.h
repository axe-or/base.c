#pragma once

#include "prelude.h"
#include "memory.h"
#include "utf8.h"

// Helper to use with printf "%.*s"
#define FmtString(str_) (int)((str_).len), (str_).data

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

// Create substring from a cstring
String str_from(cstring data);

// Create substring from a piece of a cstring
String str_from_range(cstring data, isize start, isize length);

// Create substring from a raw slice of bytes
String str_from_bytes(byte const* data, isize length);

// Get a sub string, starting at `start` with `length`
String str_sub(String s, isize start, isize length);

// Clone a string
String str_clone(String s, Mem_Allocator allocator);

// Destroy a string
void str_destroy(String s, Mem_Allocator allocator);

// Concatenate 2 strings
String str_concat(String a, String b, Mem_Allocator allocator);

// Check if 2 strings are equal
bool str_eq(String a, String b);

// Strip leading and trailing codepoints that belong to the cutset
String str_strip(String s, String cutset);

// Strip leading codepoints that belong to the cutset
String str_strip_leading(String s, String cutset);

// Strip trailing codepoints that belong to the cutset
String str_strip_trailing(String s, String cutset);

// Get an utf8 iterator form string
UTF8_Iterator str_iterator(String s);

#ifdef BASE_C_IMPLEMENTATION

static const String EMPTY = {0};

String str_from(cstring data){
	String s = {
		.data = (byte const *)data,
		.len = cstring_len(data),
	};
	return s;
}

String str_from_bytes(byte const* data, isize length){
	String s = {
		.data = (byte const *)data,
		.len = length,
	};
	return s;
}

String str_concat(String a, String b, Mem_Allocator allocator){
	byte* data = New(byte, a.len + b.len, allocator);
	String s = {0};
	if(data != NULL){
		mem_copy(&data[0], a.data, a.len);
		mem_copy(&data[a.len], b.data, b.len);
		s.data = data;
		s.len = a.len + b.len;
	}
	return s;
}

String str_from_range(cstring data, isize start, isize length){
	String s = {
		.data = (byte const *)&data[start],
		.len = length,
	};
	return s;
}

// TODO: Handle length in codepoint count
String str_sub(String s, isize start, isize length){
	if(start >= s.len || start < 0 || start + length > s.len){ return EMPTY; }

	String sub = {
		.data = &s.data[start],
		.len = length,
	};

	return sub;
}

String str_clone(String s, Mem_Allocator allocator){
	char* mem = New(char, s.len, allocator);
	if(mem == NULL){ return EMPTY; }
	return (String){
		.data = (byte const *)mem,
		.len = s.len,
	};
}

bool str_eq(String a, String b){
	if(a.len != b.len){ return false; }

	for(isize i = 0; i < a.len; i += 1){
		if(a.data[i] != b.data[i]){ return false; }
	}

	return true;
}

UTF8_Iterator str_iterator(String s){
	return (UTF8_Iterator){
		.current = 0,
		.data_length = s.len,
		.data = s.data,
	};
}

void str_destroy(String s, Mem_Allocator allocator){
	mem_free(allocator, (void*)s.data);
}

String str_strip(String s, String cutset){
	unimplemented();
}

#define MAX_CUTSET_LEN 64

String str_strip_leading(String s, String cutset){
	debug_assert(cutset.len <= MAX_CUTSET_LEN, "Cutset string exceeds MAX_CUTSET_LEN");

	Codepoint set[MAX_CUTSET_LEN] = {0};
	isize set_len = 0;
	isize cut_until = 0;

	decode_cutset: {
		Codepoint c; i8 n;
		UTF8_Iterator iter = str_iterator(cutset);

		isize i = 0;
		while(utf8_iter_next(&iter, &c, &n) && i < MAX_CUTSET_LEN){
			set[i] = c;
			i += 1;
		}
		set_len = i;
	}
	printf("SET LENGTH: %ld\n", set_len);

	strip_cutset: {
		Codepoint c; i8 n;
		UTF8_Iterator iter = str_iterator(s);

		while(utf8_iter_next(&iter, &c, &n)){
			bool to_be_cut = false;
			for(isize i = 0; i < set_len; i += 1){
				if(set[i] == c){
					to_be_cut = true;
					break;
				}
			}

			if(to_be_cut){
				cut_until += n;
			}
			else {
				break; // Reached first Codepoint that isn't in cutset
			}

		}
	}

	return str_sub(s, cut_until, s.len - cut_until);
}

String str_strip_trailing(String s, String cutset){
	unimplemented();
}

#endif
