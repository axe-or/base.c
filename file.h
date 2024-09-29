#pragma once

#include "prelude.h"
#include "memory.h"
#include "string.h"

// Reads whole file into memory, it allocates one extra byte implicitly and sets
// it to 0, to allow for better interop with cstrings.
Bytes file_read_all(String path, Mem_Allocator allocator);

// Write n bytes of data to file at path. Returns number of bytes written
// (negative means error).
isize file_write(String path, byte const* data, isize n);

// Append n bytes of data to file at path. Returns number of bytes added
// (negative means error).
isize file_append(String path, byte const* data, isize n);

#ifdef BASE_C_IMPLEMENTATION

#include <stdio.h>
#define MAX_PATH_LEN 4096

static inline
isize _file_add_content(cstring path, cstring flags, byte const* data, isize nbytes){
	FILE* f = fopen(path, flags);
	if(f == NULL){ return -1; }

	isize written = fwrite(data, 1, nbytes, f);
	fclose(f);
	return written;
}

isize file_write(String path, byte const* data, isize n){
	char path_buf[MAX_PATH_LEN] = {0};
	mem_copy(path_buf, path.data, min(path.len, MAX_PATH_LEN - 1));
	return _file_add_content(path_buf, "wb", data, n);
}

isize file_append(String path, byte const* data, isize n){
	char path_buf[MAX_PATH_LEN] = {0};
	mem_copy(path_buf, path.data, min(path.len, MAX_PATH_LEN - 1));
	return _file_add_content(path_buf, "ab", data, n);
}

Bytes file_read_all(String path, Mem_Allocator allocator){
	static const Bytes error = {0, 0};

	char path_buf[MAX_PATH_LEN] = {0};
	mem_copy(path_buf, path.data, min(path.len, MAX_PATH_LEN - 1));

	FILE* f = fopen(path_buf, "rb");
	if(f == NULL){ goto error_exit; }

	fseek(f, 0, SEEK_END);
	isize end = ftell(f);
	rewind(f);
	isize start = ftell(f);

	isize size = end - start;
	if(size <= 0){ return error; }

	byte* data = New(byte, size + 1, allocator);
	if(data == NULL){ goto error_exit; }
	data[size] = 0;

	size = fread(data, 1, size, f);
	fclose(f);
	return (Bytes){ .data = data, .len = size };

error_exit:
	if(f != NULL) { fclose(f); }
	return error;
}
#endif
