#define BASE_C_IMPLEMENTATION 1
#include "memory.h"
#include "io.h"
#include "file.h"
#include "heap_allocator.h"
#include <stdio.h>

int main(){
	Bytes buf = read_whole_file(str_from("main.c"), heap_allocator());
	printf("%.*s\n", FMT_STRING(buf));
	mem_free(heap_allocator(), buf.data);
}
