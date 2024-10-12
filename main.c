#include "base.h"
#include <stdio.h>

int main(int argc, char** argv){
	printf(
		"Context:\n"
		"    Compiler: %s\n"
		"    Endianess: %s\n",

		context_compiler_vendor_names[context_compiler_vendor],
		context_endianness == Ctx_Endian_Big ? "Big" : "Little"
	);

}
