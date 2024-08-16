#include "bytes_buffer.h"

bool buffer_init(Bytes_Buffer* bb, Mem_Allocator allocator, isize initial_cap){
	bb->allocator = allocator;
	bb->data = New(byte, initial_cap, allocator);

	if(bb->data != NULL){
		bb->cap = initial_cap;
		bb->len = 0;
		bb->last_read = 0;
		return true;
	} else {
		return false;
	}
}

void buffer_destroy(Bytes_Buffer* bb){
	mem_free(bb->allocator, bb->data);
	bb->data = 0;
}

// Clear buffer's read bytes, this shifts the buffer's memory back to its base.
void buffer_clean_read_bytes(Bytes_Buffer* bb){
	byte* data = buffer_bytes(bb);
	mem_copy(bb->data, data, bb->len);
	bb->last_read = 0;
}

byte* buffer_bytes(Bytes_Buffer* bb){
	return &bb->data[bb->last_read];
}

// Read bytes from the buffer, pushing its `read` pointer forward. Returns number of bytes read.
isize buffer_read(Bytes_Buffer* bb, byte* dest, isize size){
	if(bb->len == 0){ return 0; }
	isize n = Min(size, bb->len);
	mem_copy(dest, &bb->data[bb->last_read], n);
	bb->last_read += n;
	bb->len -= n;
	return n;
}

bool buffer_write(Bytes_Buffer* bb, byte const * bytes, isize len){
	if(bb->len + bb->last_read + len > bb->cap){
		bool status = buffer_resize(bb, bb->cap * 2);
		if(!status){ return false; }
	}
	mem_copy(&bb->data[bb->last_read + bb->len], bytes, len);
	bb->len += len;
	return true;
}

bool buffer_resize(Bytes_Buffer* bb, isize new_size){
	byte* new_data = New(byte, new_size, bb->allocator);
	if(new_data == NULL){ return false; }

	mem_copy(new_data, bb->data, Min(new_size, bb->len));
	mem_free(bb->allocator, bb->data);
	bb->data = new_data;
	bb->cap = new_size;

	return true;
}

void buffer_reset(Bytes_Buffer* bb){
	bb->len = 0;
	mem_set(bb->data, 0, bb->cap);
}


