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
	printf(
		"Context:\n"
		"    Compiler: %s\n"
		"    Endianess: %s\n",

		context_compiler_vendor_names[context_compiler_vendor],
		context_endianness == Ctx_Endian_Big ? "Big" : "Little"
	);

}
