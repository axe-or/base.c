#pragma once

#include "io.h"

#include "prelude.h"
#include "mem.h"

typedef struct {
	byte* data;
	isize cap;       // Total capacity
	isize last_read; // Offset of last read position
	isize len;       // Number of bytes after last_read
	Mem_Allocator allocator;
} Bytes_Buffer;

// Init a builder with a capacity, returns success status
bool buffer_init(Bytes_Buffer* bb, Mem_Allocator allocator, isize initial_cap);

// Get remaining free size given the current capacity
static inline
isize buffer_remaining(Bytes_Buffer* bb){
	return bb->cap - (bb->last_read + bb->len);
}

// Destroy a builder
void buffer_destroy(Bytes_Buffer* bb);

// Resize builder to have specified capacity, returns success status.
bool buffer_resize(Bytes_Buffer* bb, isize new_size);

// Resets builder's data, does not de-allocate
void buffer_reset(Bytes_Buffer* bb);

// Clear buffer's read bytes, this shifts the buffer's memory back to its base.
void buffer_clean_read_bytes(Bytes_Buffer* bb);

// Read bytes from the buffer, pushing its `read` pointer forward. Returns number of bytes read.
isize buffer_read(Bytes_Buffer* bb, byte* dest, isize size);

// Push bytes to the end of builder, returns success status
bool buffer_write(Bytes_Buffer* bb, byte const * b, isize len);

// Current unread bytes, this pointer becomes invalid as soon as the buffer is modified.
byte* buffer_bytes(Bytes_Buffer* bb);

// IO_Writer buffer_writer(Bytes_Buffer* bb);
// IO_Reader buffer_reader(Bytes_Buffer* bb);
