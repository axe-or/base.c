#define BASE_C_IMPLEMENTATION 1
#include "base.h"
#include <stdio.h>

int main(){
	Bytes b = file_read_all(str_from("main.c"), heap_allocator());
	printf("%.*s", FMT_STRING(b));
	return 0;
}
