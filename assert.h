#pragma once

#include "prelude.h"

// Crash if `pred` is false, this is disabled in non-debug builds
void debug_assert(bool pred, cstring msg);

// Crash if `pred` is false, this is always enabled
void panic_assert(bool pred, cstring msg);

// Crash the program with a fatal error
noreturn void panic(char* const msg);

// Crash the program due to unimplemented code paths, this should *only* be used
// during development
noreturn void unimplemented();

#ifdef BASE_C_IMPLEMENTATION
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
#endif
