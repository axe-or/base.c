#include "./assert.h"

#include <stdio.h>
#include <stdlib.h>

void debug_assert(bool pred, cstring msg){
	#ifdef NDEBUG
		(void)pred; (void)msg;
	#else
	if(!pred){
		fprintf(stderr, "Failed assert: %s\n", msg);
		abort();
	}
	#endif
}

void panic_assert(bool pred, cstring msg){
	if(!pred){
		fprintf(stderr, "Failed assert: %s\n", msg);
		abort();
	}
}

noreturn
void panic(char* const msg){
	fprintf(stderr, "Panic: %s\n", msg);
	abort();
}

noreturn
void unimplemented(){
	fprintf(stderr, "Unimplemented code\n");
	abort();
}