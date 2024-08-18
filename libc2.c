/* Convenience file to have a translation unit that implements all of libc2's utilities. */
#define LIBC2_IMPLEMENTATION 1
#include "arena_allocator.h"
#include "assert.h"
#include "bytes_buffer.h"
#include "heap_allocator.h"
#include "io.h"
#include "mem.h"
#include "prelude.h"
#include "string.h"
#include "testing.h"
#include "utf8.h"
#undef LIBC2_IMPLEMENTATION
