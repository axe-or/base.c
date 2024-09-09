#pragma once

// Interface ///////////////////////////////////////////////////////////////////
#include "prelude.h"
#include "memory.h"
#include "string.h"

byte* read_whole_file(String path, Mem_Allocator allocator);

isize write_to_file(String path, byte* data, isize n);

// Implementation //////////////////////////////////////////////////////////////
#ifdef BASE_C_IMPLEMENTATION

#include <stdio.h>

#define MAX_PATH_LEN 4096

byte* read_whole_file(String path, Mem_Allocator allocator){
	char path_buf[MAX_PATH_LEN] = {0};
	mem_copy(path_buf, path.data, path.len);
}
#endif
