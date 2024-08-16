#pragma once

#include "prelude.h"
#include "mem.h"

typedef struct {
	isize offset;
	isize capacity;
	byte* data;
} Mem_Arena;

void arena_init(Mem_Arena* a, byte* data, isize len);
void arena_destroy(Mem_Arena *a);
Mem_Allocator arena_allocator(Mem_Arena* a);

