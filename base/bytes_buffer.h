#pragma once

#include "prelude.h"
#include "io.h"
#include "memory.h"

typedef struct {
	byte* data;
	isize cap;       // Total capacity
	isize last_read; // Offset of last read position
	isize len;       // Number of bytes after last_read
	Mem_Allocator allocator;
} Bytes_Buffer;

// Init a buffer with a capacity, returns success status
bool buffer_init(Bytes_Buffer* bb, Mem_Allocator allocator, isize initial_cap);

// Get remaining free size given the current capacity
static inline
isize buffer_remaining(Bytes_Buffer* bb){
	return bb->cap - (bb->last_read + bb->len);
}

// Destroy a buffer
void buffer_destroy(Bytes_Buffer* bb);

// Resize buffer to have specified capacity, returns success status.
bool buffer_resize(Bytes_Buffer* bb, isize new_size);

// Resets buffer's data, does not de-allocate
void buffer_reset(Bytes_Buffer* bb);

// Clear buffer's read bytes, this shifts the buffer's memory back to its base.
void buffer_clean_read_bytes(Bytes_Buffer* bb);

// Read bytes from the buffer, pushing its `read` pointer forward. Returns number of bytes read.
isize buffer_read(Bytes_Buffer* bb, byte* dest, isize size);

// Push bytes to the end of buffer, returns success status
bool buffer_write(Bytes_Buffer* bb, byte const* b, isize len);

// Current unread bytes, this pointer becomes invalid as soon as the buffer is modified.
byte* buffer_bytes(Bytes_Buffer* bb);

// Get bytes buffer as a IO stream
IO_Stream buffer_stream(Bytes_Buffer* bb);
