#define BASE_C_IMPLEMENTATION 1
#include "memory.h"
#include "io.h"
#include "file.h"
#include "heap_allocator.h"
#include <stdio.h>

int main(){
	Bytes b = file_read_all(str_from("main.c"), heap_allocator());
	printf("%.*s", FMT_STRING(b));
	return 0;
}
