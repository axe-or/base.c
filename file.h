#pragma once

// Interface ///////////////////////////////////////////////////////////////////
#include "prelude.h"
#include "memory.h"
#include "string.h"

// Reads whole file into memory, it allocates one extra byte implicitly, to
// allow for better interop with cstrings.
Bytes read_whole_file(String path, Mem_Allocator allocator);

isize write_to_file(String path, byte* data, isize n);

// Implementation //////////////////////////////////////////////////////////////
#ifdef BASE_C_IMPLEMENTATION

#include <stdio.h>

#define MAX_PATH_LEN 4096

Bytes read_whole_file(String path, Mem_Allocator allocator){
	static const Bytes error = {0, 0};

	char path_buf[MAX_PATH_LEN] = {0};
	mem_copy(path_buf, path.data, path.len);

	FILE* f = fopen(path_buf, "rb");
	if(f == NULL){ return error; }

	fseek(f, 0, SEEK_END);
	isize end = ftell(f);
	rewind(f);
	isize start = ftell(f);

	isize size = end - start;
	if(size <= 0){ return error; }

	byte* data = New(byte, size + 1, allocator);
	if(data == NULL){ return error; }
	data[size] = 0;

	fread(data, 1, size, f);
	return (Bytes){ .data = data, .len = size };
}
#endif
