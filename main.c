#define BASE_C_IMPLEMENTATION 1
#include "prelude.h"
#include "heap_allocator.h"
#include "command_line_opt.h"
#include "string.h"
#include <stdio.h>

#include "context.h"
// #define container_name Int_Array
// #define container_type_param i64
// #define container_prefix ia
// #include "generic/dynamic_array.h"

int main(int argc, char** argv){
	switch(context_endianness){
	case Endian_Big: printf("%08x Big\n", context_endianness);
	case Endian_Little:printf("%08x Little\n", context_endianness);
	}
}
