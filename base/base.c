/* Single compilation unit for whole library */
/* File: src/testing.c */

/* File: src/utf8.c */
#include "./utf8.h"

#define SURROGATE1 ((i32)0xd800)
#define SURROGATE2 ((i32)0xdfff)

#define MASK2 (0x1f) /* 0001_1111 */
#define MASK3 (0x0f) /* 0000_1111 */
#define MASK4 (0x07) /* 0000_0111 */

#define MASKX (0x3f) /* 0011_1111 */

#define SIZE2 (0xc0) /* 110x_xxxx */
#define SIZE3 (0xe0) /* 1110_xxxx */
#define SIZE4 (0xf0) /* 1111_0xxx */

#define CONT  (0x80)  /* 10xx_xxxx */

#define CONTINUATION1 (0x80)
#define CONTINUATION2 (0xbf)

static inline
bool is_continuation_byte(Codepoint c){
	return (c >= CONTINUATION1) && (c <= CONTINUATION2);
}

UTF8_Encode_Result utf8_encode(Codepoint c){
	UTF8_Encode_Result res = {0};

	if((c >= CONTINUATION1 && c <= CONTINUATION2) ||
	   (c >= SURROGATE1 && c <= SURROGATE2) ||
	   (c > UTF8_RANGE4))
	{
		return UTF8_ERROR_ENCODED;
	}

	if(c <= UTF8_RANGE1){
		res.len = 1;
		res.bytes[0] = c;
	}
	else if(c <= UTF8_RANGE2){
		res.len = 2;
		res.bytes[0] = SIZE2 | ((c >> 6) & MASK2);
		res.bytes[1] = CONT  | ((c >> 0) & MASKX);
	}
	else if(c <= UTF8_RANGE3){
		res.len = 3;
		res.bytes[0] = SIZE3 | ((c >> 12) & MASK3);
		res.bytes[1] = CONT  | ((c >> 6) & MASKX);
		res.bytes[2] = CONT  | ((c >> 0) & MASKX);
	}
	else if(c <= UTF8_RANGE4){
		res.len = 4;
		res.bytes[0] = SIZE4 | ((c >> 18) & MASK4);
		res.bytes[1] = CONT  | ((c >> 12) & MASKX);
		res.bytes[2] = CONT  | ((c >> 6)  & MASKX);
		res.bytes[3] = CONT  | ((c >> 0)  & MASKX);
	}
	return res;
}

static const UTF8_Decode_Result DECODE_ERROR = { .codepoint = UTF8_ERROR, .len = 0 };

UTF8_Decode_Result utf8_decode(byte const* buf, isize len){
	UTF8_Decode_Result res = {0};
	if(buf == NULL || len <= 0){ return DECODE_ERROR; }

	u8 first = buf[0];

	if((first & CONT) == 0){
		res.len = 1;
		res.codepoint |= first;
	}
	else if ((first & ~MASK2) == SIZE2 && len >= 2){
		res.len = 2;
		res.codepoint |= (buf[0] & MASK2) << 6;
		res.codepoint |= (buf[1] & MASKX) << 0;
	}
	else if ((first & ~MASK3) == SIZE3 && len >= 3){
		res.len = 3;
		res.codepoint |= (buf[0] & MASK3) << 12;
		res.codepoint |= (buf[1] & MASKX) << 6;
		res.codepoint |= (buf[2] & MASKX) << 0;
	}
	else if ((first & ~MASK4) == SIZE4 && len >= 4){
		res.len = 4;
		res.codepoint |= (buf[0] & MASK4) << 18;
		res.codepoint |= (buf[1] & MASKX) << 12;
		res.codepoint |= (buf[2] & MASKX) << 6;
		res.codepoint |= (buf[3] & MASKX) << 0;
	}
	else {
		return DECODE_ERROR;
	}

	// Validation
	if(res.codepoint >= SURROGATE1 && res.codepoint <= SURROGATE2){
		return DECODE_ERROR;
	}
	if(res.len > 1 && (buf[1] < CONTINUATION1 || buf[1] > CONTINUATION2)){
		return DECODE_ERROR;
	}
	if(res.len > 2 && (buf[2] < CONTINUATION1 || buf[2] > CONTINUATION2)){
		return DECODE_ERROR;
	}
	if(res.len > 3 && (buf[3] < CONTINUATION1 || buf[3] > CONTINUATION2)){
		return DECODE_ERROR;
	}

	return res;
}

// Steps iterator forward and puts Codepoint and Length advanced into pointers,
// returns false when finished.
bool utf8_iter_next(UTF8_Iterator* iter, Codepoint* r, i8* len){
	if(iter->current >= iter->data_length){ return 0; }

	UTF8_Decode_Result res = utf8_decode(&iter->data[iter->current], iter->data_length);
	*r = res.codepoint;
	*len = res.len;

	if(res.codepoint == DECODE_ERROR.codepoint){
		*len = res.len + 1;
	}

	iter->current += res.len;

	return 1;
}

// Steps iterator backward and puts Codepoint and its length into pointers,
// returns false when finished.
bool utf8_iter_prev(UTF8_Iterator* iter, Codepoint* r, i8* len){
	if(iter->current <= 0){ return false; }

	iter->current -= 1;
	while(is_continuation_byte(iter->data[iter->current])){
		iter->current -= 1;
	}

	UTF8_Decode_Result res = utf8_decode(&iter->data[iter->current], iter->data_length - iter->current);
	*r = res.codepoint;
	*len = res.len;
	return true;
}



#undef SURROGATE2
#undef SURROGATE1
#undef MASK2
#undef MASK3
#undef MASK4
#undef MASKX
#undef SIZE2
#undef SIZE3
#undef SIZE4
#undef CONT
#undef CONTINUATION1
#undef CONTINUATION2


/* File: src/assert.c */
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
/* File: src/memory.c */
#include "./memory.h"

void mem_set(void* p, byte val, isize nbytes){
	__builtin_memset(p, val, nbytes);
}

void mem_copy(void* dest, void const * src, isize nbytes){
	__builtin_memmove(dest, src, nbytes);
}

void mem_copy_no_overlap(void* dest, void const * src, isize nbytes){
	__builtin_memcpy(dest, src, nbytes);
}

uintptr align_forward_ptr(uintptr p, uintptr a){
	debug_assert(mem_valid_alignment(a), "Invalid memory alignment");
	uintptr mod = p & (a - 1);
	if(mod > 0){
		p += (a - mod);
	}
	return p;
}

uintptr align_forward_size(isize p, isize a){
	debug_assert(a > 0, "Invalid size alignment");
	isize mod = p % a;
	if(mod > 0){
		p += (a - mod);
	}
	return p;
}

i32 allocator_query_capabilites(Mem_Allocator allocator, i32* capabilities){
	if(capabilities == NULL){ return 0; }
	allocator.func(allocator.data, Mem_Op_Query, NULL, 0, 0, capabilities);
	return *capabilities;
}

void* mem_alloc(Mem_Allocator allocator, isize size, isize align){
	void* ptr = allocator.func(allocator.data, Mem_Op_Alloc, NULL, size, align, NULL);
	if(ptr != NULL){
		mem_set(ptr, 0, size);
	}
	return ptr;
}

void* mem_resize(Mem_Allocator allocator, void* ptr, isize new_size){
	void* new_ptr = allocator.func(allocator.data, Mem_Op_Resize, ptr, new_size, 0, NULL);
	return new_ptr;
}

void mem_free_ex(Mem_Allocator allocator, void* p, isize align){
	if(p == NULL){ return; }
	allocator.func(allocator.data, Mem_Op_Free, p, 0, align, NULL);
}

void mem_free(Mem_Allocator allocator, void* p){
	mem_free_ex(allocator, p, 0);
}

void mem_free_all(Mem_Allocator allocator){
	allocator.func(allocator.data, Mem_Op_Free_All, NULL, 0, 0, NULL);
}
/* File: src/list.c */
#include "./list.h"

// Insert new_node between 2 existing nodes
static void _list_add(List_Node* prev, List_Node* next, List_Node* new_node){
	new_node->next = next;
	new_node->prev = prev;
	if(next != NULL){ next->prev = new_node; }
	if(prev != NULL){ prev->next = new_node; }
}

static void _list_del(List_Node* node){
	List_Node* prev = node->prev;
	List_Node* next = node->next;
	if(next != NULL){ next->prev = prev; }
	if(prev != NULL){ next->prev = next; }
}

void list_add(List_Node* target, List_Node* new_node){
	_list_add(target, target->next, new_node);
}

void list_del(List_Node* node){
	_list_del(node);
}
/* File: src/heap_allocator.c */
#include "./heap_allocator.h"

#include <stdlib.h>

static
void* heap_alloc(isize nbytes, isize align){
	void* data = malloc(nbytes);
	uintptr aligned = align_forward_ptr((uintptr)data, (uintptr)align);
	if(aligned == (uintptr)data){
		return data;
	} else {
		debug_assert(false, "Could not allocate memory with proper alignment");
		free(data);
		return NULL;
	}
}

static
void* heap_allocator_func(
	void* impl,
	enum Allocator_Op op,
	void* old_ptr,
	isize size, isize align,
	i32* capabilities
){
	(void)impl;

	switch(op){
		case Mem_Op_Alloc: {
			return heap_alloc(size, align);
		} break;

		case Mem_Op_Free: {
			free(old_ptr);
		} break;

		case Mem_Op_Resize: {} break;

		case Mem_Op_Free_All: {} break;

		case Mem_Op_Query: {
			*capabilities = Allocator_Alloc_Any | Allocator_Free_Any;
		} break;
	}

	return NULL;
}

Mem_Allocator heap_allocator(){
	return (Mem_Allocator){
		.func = heap_allocator_func,
		.data = NULL,
	};
}
/* File: src/command_line_opt.c */
#include "./command_line_opt.h"

static inline
bool _is_whitespace(char c){
	return c == ' ' || c == '\t' || c == '\v' || c == '\r' || c == '\n';
}

static inline
bool _is_numeric(char c){
	return c >= '0' && c <= '9';
}

CLI_Arg cli_parse_arg(String arg){
	String s = str_trim(arg, str_from(" \t\v\r\n"));
	CLI_Arg res = {
		.value = str_from(""),
		.key = str_from(""),
		.type = CLI_Flag_None,
	};

	if(s.len > 1){
		bool is_flag = s.data[0] == '-' &&
			!_is_whitespace(s.data[1]) &&
			!_is_numeric(s.data[1]) &&
			s.data[1] != ':';

		if(is_flag){
			res.type = CLI_Flag_Toggle;
			isize colon_pos = -1;

			for(isize i = 1; i < s.len; i ++){
				if(s.data[i] == ':'){
					colon_pos = i;
					break;
				}
			}

			isize keylen = max(colon_pos, 1);
			res.key = str_sub(s, 1, keylen);
			if(colon_pos > 0){
				res.value = str_sub(s, colon_pos + 1, s.len - colon_pos - 1);
				res.type = CLI_Flag_Key_Value;
			}
		}
		else {
			res.value = s;
		}
	}
	else {
		res.value = s;
	}
	return res;
}
/* File: src/string.c */
#include "./string.h"

static const String EMPTY = {0};

bool str_empty(String s){
	return s.len == 0 || s.data == NULL;
}

String str_from(cstring data){
	String s = {
		.data = (byte const *)data,
		.len = cstring_len(data),
	};
	return s;
}

String str_from_bytes(byte const* data, isize length){
	String s = {
		.data = (byte const *)data,
		.len = length,
	};
	return s;
}

String str_concat(String a, String b, Mem_Allocator allocator){
	byte* data = New(byte, a.len + b.len, allocator);
	String s = {0};
	if(data != NULL){
		mem_copy(&data[0], a.data, a.len);
		mem_copy(&data[a.len], b.data, b.len);
		s.data = data;
		s.len = a.len + b.len;
	}
	return s;
}

String str_from_range(cstring data, isize start, isize length){
	String s = {
		.data = (byte const *)&data[start],
		.len = length,
	};
	return s;
}

isize str_codepoint_count(String s){
	UTF8_Iterator it = str_iterator(s);

	isize count = 0;
	Codepoint c; i8 len;
	while(utf8_iter_next(&it, &c, &len)){
		count += 1;
	}
	return count;
}


isize str_codepoint_offset(String s, isize n){
	UTF8_Iterator it = str_iterator(s);
	
	isize acc = 0;

	Codepoint c; i8 len;
	do {
		if(acc == n){ break; }
		acc += 1;
	} while(utf8_iter_next(&it, &c, &len));

	return it.current;
}

// TODO: Handle length in codepoint count
String str_sub(String s, isize start, isize byte_count){
	if(start < 0 || byte_count < 0 || (start + byte_count) > s.len){ return EMPTY; }

	String sub = {
		.data = &s.data[start],
		.len = byte_count,
	};

	return sub;
}

String str_clone(String s, Mem_Allocator allocator){
	char* mem = New(char, s.len, allocator);
	if(mem == NULL){ return EMPTY; }
	return (String){
		.data = (byte const *)mem,
		.len = s.len,
	};
}

bool str_eq(String a, String b){
	if(a.len != b.len){ return false; }

	for(isize i = 0; i < a.len; i += 1){
		if(a.data[i] != b.data[i]){ return false; }
	}

	return true;
}

UTF8_Iterator str_iterator(String s){
	return (UTF8_Iterator){
		.current = 0,
		.data_length = s.len,
		.data = s.data,
	};
}

UTF8_Iterator str_iterator_reversed(String s){
	return (UTF8_Iterator){
		.current = s.len,
		.data_length = s.len,
		.data = s.data,
	};
}

void str_destroy(String s, Mem_Allocator allocator){
	mem_free(allocator, (void*)s.data);
}

#define MAX_CUTSET_LEN 64

String str_trim(String s, String cutset){
	String st = str_trim_leading(str_trim_trailing(s, cutset), cutset);
	return st;
}

String str_trim_leading(String s, String cutset){
	debug_assert(cutset.len <= MAX_CUTSET_LEN, "Cutset string exceeds MAX_CUTSET_LEN");

	Codepoint set[MAX_CUTSET_LEN] = {0};
	isize set_len = 0;
	isize cut_after = 0;

	decode_cutset: {
		Codepoint c; i8 n;
		UTF8_Iterator iter = str_iterator(cutset);

		isize i = 0;
		while(utf8_iter_next(&iter, &c, &n) && i < MAX_CUTSET_LEN){
			set[i] = c;
			i += 1;
		}
		set_len = i;
	}

	strip_cutset: {
		Codepoint c; i8 n;
		UTF8_Iterator iter = str_iterator(s);

		while(utf8_iter_next(&iter, &c, &n)){
			bool to_be_cut = false;
			for(isize i = 0; i < set_len; i += 1){
				if(set[i] == c){
					to_be_cut = true;
					break;
				}
			}

			if(to_be_cut){
				cut_after += n;
			}
			else {
				break; // Reached first Codepoint that isn't in cutset
			}

		}
	}

	return str_sub(s, cut_after, s.len - cut_after);
}

String str_trim_trailing(String s, String cutset){
	debug_assert(cutset.len <= MAX_CUTSET_LEN, "Cutset string exceeds MAX_CUTSET_LEN");

	Codepoint set[MAX_CUTSET_LEN] = {0};
	isize set_len = 0;
	isize cut_until = s.len;

	decode_cutset: {
		Codepoint c; i8 n;
		UTF8_Iterator iter = str_iterator(cutset);

		isize i = 0;
		while(utf8_iter_next(&iter, &c, &n) && i < MAX_CUTSET_LEN){
			set[i] = c;
			i += 1;
		}
		set_len = i;
	}

	strip_cutset: {
		Codepoint c; i8 n;
		UTF8_Iterator iter = str_iterator_reversed(s);

		while(utf8_iter_prev(&iter, &c, &n)){
			bool to_be_cut = false;
			for(isize i = 0; i < set_len; i += 1){
				if(set[i] == c){
					to_be_cut = true;
					break;
				}
			}

			if(to_be_cut){
				cut_until -= n;
			}
			else {
				break; // Reached first Codepoint that isn't in cutset
			}

		}
	}

	return str_sub(s, 0, cut_until);
}
/* File: src/spinlock.c */
#include "./spinlock.h"

void spinlock_acquire(Spinlock* l){
	for(;;){
		if(!atomic_exchange_explicit(&l->_state, SPINLOCK_LOCKED, memory_order_acquire)){
			break;
		}
		/* Busy wait while locked */
		while(atomic_load_explicit(&l->_state, memory_order_relaxed));
	}
}

bool spinlock_try_acquire(Spinlock* l){
    return !atomic_exchange_explicit(&l->_state, SPINLOCK_LOCKED, memory_order_acquire);
}

void spinlock_release(Spinlock* l){
	atomic_store(&l->_state, SPINLOCK_UNLOCKED);
}

/* File: src/fnv_hash.c */
#include "./fnv_hash.h"

i32 hash_fnv32_ex(void const* data, isize len, i32 seed){
	i32 h = seed;
	byte const* buf = (byte const*)data;
	for(isize i = 0; i < len; i ++){
		h = h ^ buf[i];
		h = h * 0x01000193;
	}
	return h;
}

i64 hash_fnv64_ex(void const* data, isize len, i64 seed){
	i64 h = seed;
	byte const* buf = (byte const*)data;
	for(isize i = 0; i < len; i ++){
		h = h ^ buf[i];
		h = h * 0x00000100000001b3;
	}
	return h;
}

i64 hash_fnv64(void const* data, isize len){
	return hash_fnv64_ex(data, len, (i32)0x811c9dc5);
}

i32 hash_fnv32(void const* data, isize len){
	return hash_fnv64_ex(data, len, (i64)0xcbf29ce484222325);
}

/* File: src/bytes_buffer.c */
#include "./bytes_buffer.h"

static inline
isize buffer_io_func(void* impl, IO_Operation op, byte* data, isize len){
	Bytes_Buffer* bb = (Bytes_Buffer*)(impl);
	switch(op){
	case IO_Op_Query: {
		return IO_Op_Read | IO_Op_Write;
	} break;

	case IO_Op_Read: {
		return buffer_read(bb, data, len);
	} break;

	case IO_Op_Write: {
		return buffer_write(bb, data, len);
	} break;
	}

	return 0;
}

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
	isize n = min(size, bb->len);
	mem_copy(dest, &bb->data[bb->last_read], n);
	bb->last_read += n;
	bb->len -= n;
	return n;
}

bool buffer_write(Bytes_Buffer* bb, byte const* bytes, isize len){
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

	mem_copy(new_data, bb->data, min(new_size, bb->len));
	mem_free(bb->allocator, bb->data);
	bb->data = new_data;
	bb->cap = new_size;

	return true;
}

void buffer_reset(Bytes_Buffer* bb){
	bb->len = 0;
	mem_set(bb->data, 0, bb->cap);
}

IO_Stream buffer_stream(Bytes_Buffer* bb){
	IO_Stream s = {
		.impl = bb,
		.func = buffer_io_func,
	};
	return s;
}
/* File: src/io.c */
#include "./io.h"
#include "./assert.h"

i8 io_query_stream(IO_Stream s){
	return s.func(s.impl, IO_Op_Query, NULL, 0);
}

isize io_read(IO_Reader r, byte* buf, isize buflen){
	IO_Stream s = r._stream;
	return s.func(s.impl, IO_Op_Read, buf, buflen);
}

isize io_write(IO_Writer w, byte const* buf, isize buflen){
	IO_Stream s = w._stream;
	return s.func(s.impl, IO_Op_Write, (byte*)(buf), buflen);
}

IO_Reader io_to_reader(IO_Stream s){
	i8 cap = io_query_stream(s);
	debug_assert(cap & IO_Op_Read, "Stream does not support reading.");
	IO_Reader r = { ._stream = s };
	return r;
}

IO_Writer io_to_writer(IO_Stream s){
	i8 cap = io_query_stream(s);
	debug_assert(cap & IO_Op_Write, "Stream does not support writing.");
	IO_Writer w = { ._stream = s };
	return w;
}

/* File: src/arena_allocator.c */
#include "arena_allocator.h"

#include "./assert.h"

static
uintptr arena_required_mem(uintptr cur, isize nbytes, isize align){
	debug_assert(mem_valid_alignment(align), "Alignment must be a power of 2");
	uintptr_t aligned  = align_forward_ptr(cur, align);
	uintptr_t padding  = (uintptr)(aligned - cur);
	uintptr_t required = padding + nbytes;
	return required;
}

static
void *arena_alloc(Mem_Arena* a, isize size, isize align){
	uintptr base = (uintptr)a->data;
	uintptr current = (uintptr)base + (uintptr)a->offset;

	uintptr available = (uintptr)a->capacity - (current - base);
	uintptr required = arena_required_mem(current, size, align);

	if(required > available){
		return NULL;
	}

	a->offset += required;
	void* allocation = &a->data[a->offset - size];
	return allocation;
}

static
void arena_free_all(Mem_Arena* a){
	a->offset = 0;
}

static
void* arena_allocator_func(
	void* impl,
	enum Allocator_Op op,
	void* old_ptr,
	isize size,
	isize align,
	i32* capabilities)
{
	Mem_Arena* a = impl;
	(void)old_ptr;

	switch(op){
		case Mem_Op_Alloc: {
			return arena_alloc(a, size, align);
		} break;

		case Mem_Op_Free_All: {
			arena_free_all(a);
		} break;

		case Mem_Op_Resize: {} break;

		case Mem_Op_Free: {} break;

		case Mem_Op_Query: {
			*capabilities = Allocator_Alloc_Any | Allocator_Free_All | Allocator_Align_Any;
		} break;
	}

	return NULL;
}

Mem_Allocator arena_allocator(Mem_Arena* a){
	Mem_Allocator allocator = {
		.data = a,
		.func = arena_allocator_func,
	};
	return allocator;
}

void arena_init(Mem_Arena* a, byte* data, isize len){
	a->capacity = len;
	a->data = data;
	a->offset = 0;
}

void arena_destroy(Mem_Arena* a){
	arena_free_all(a);
	a->capacity = 0;
	a->data = NULL;
}
