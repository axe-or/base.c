#pragma once

#include "memory.h"

// Get the system's heap allocator, this will typically be LibC's but could also
// be a custom one or a 3rd party one like mimalloc.
Mem_Allocator heap_allocator();

