#pragma once

#include "./prelude.h"

// Crash if `pred` is false, this is disabled in non-debug builds
void debug_assert(bool pred, cstring msg);

// Crash if `pred` is false, this is always enabled
void panic_assert(bool pred, cstring msg);

// Crash the program with a fatal error
noreturn void panic(char* const msg);

// Crash the program due to unimplemented code paths, this should *only* be used
// during development
noreturn void unimplemented();
