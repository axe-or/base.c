#define BASE_C_IMPLEMENTATION 1
#include "prelude.h"
#include "heap_allocator.h"
#include "command_line_opt.h"
#include "string.h"
#include <stdio.h>

// #define container_name Int_Array
// #define container_type_param i64
// #define container_prefix ia
// #include "generic/dynamic_array.h"

int main(int argc, char** argv){
	CLI_Arg args[512];
	for(int i = 1; i < min(argc, 512); i ++){
		args[i] = cli_parse_arg(str_from(argv[i]));
		printf("Type: %d Key: '%.*s' Val: '%.*s'\n", args[i].type, FmtString(args[i].key), FmtString(args[i].value));
	}
	return 0;
}
