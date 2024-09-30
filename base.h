#pragma once
/* Essential definitions. */

#define BASE_C_VERSION "63f5abf8e23decb2295e45f8f52455399b8a72aa"

#include <stddef.h>
#include <stdint.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stdnoreturn.h>

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef unsigned int uint;
typedef uint8_t byte;

typedef ptrdiff_t isize;
typedef size_t    usize;

typedef uintptr_t uintptr;

typedef float f32;
typedef double f64;

typedef char const * cstring;

static inline
int arch_is_big_endian(){
	union {
		u16 x;
		u8  b[2];
	} u;
	u.x = 1;
	return u.b[1] == 1;
}

static inline
void swap_bytes_raw(byte* data, isize len){
	for(isize i = 0; i < (len / 2); i += 1){
		byte temp = data[i];
		data[i] = data[len - (i + 1)];
		data[len - (i + 1)] = temp;
	}
}

#define swap_bytes(Ptr) swap_bytes_raw((byte*)(Ptr), sizeof(*(Ptr)))

_Static_assert(sizeof(f32) == 4 && sizeof(f64) == 8, "Bad float size");
_Static_assert(sizeof(isize) == sizeof(usize), "Bad (i/u)size");

#define min(A, B) ((A) < (B) ? (A) : (B))
#define max(A, B) ((A) > (B) ? (A) : (B))
#define clamp(Lo, X, Hi) min(max(Lo, X), Hi)

#define container_of(Ptr, Type, Member) \
	((Type *)(((void *)(Ptr)) - offsetof(Type, Member)))

#ifndef __cplusplus
#undef bool
typedef _Bool bool;
#endif


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

#define UTF8_RANGE1 ((i32)0x7f)
#define UTF8_RANGE2 ((i32)0x7ff)
#define UTF8_RANGE3 ((i32)0xffff)
#define UTF8_RANGE4 ((i32)0x10ffff)

typedef i32 Codepoint;

typedef struct {
	byte bytes[4];
	i8 len;
} UTF8_Encode_Result;

typedef struct {
	Codepoint codepoint;
	i8 len;
} UTF8_Decode_Result;

static const Codepoint UTF8_ERROR = 0xfffd;

static const UTF8_Encode_Result UTF8_ERROR_ENCODED = {
	.bytes = {0xef, 0xbf, 0xbd},
	.len = 3,
};

static inline
bool utf8_continuation_byte(byte b){
	return (b & 0xc0) == 0x80;
}

// Encode a unicode Codepoint
UTF8_Encode_Result utf8_encode(Codepoint c);

// Decode a codepoint from a UTF8 buffer of bytes
UTF8_Decode_Result utf8_decode(byte const* data, isize len);

typedef struct {
	byte const* data;
	isize data_length;
	isize current;
} UTF8_Iterator;

// Steps iterator forward and puts Codepoint and Length advanced into pointers,
// returns false when finished.
bool utf8_iter_next(UTF8_Iterator* iter, Codepoint* r, i8* len);

#ifdef BASE_C_IMPLEMENTATION

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

static inline
bool is_continuation_byte(Codepoint c){
	return (c >= CONTINUATION1) && (c <= CONTINUATION2);
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

#endif
/* Intrusive circular doubly linked list. This is similar to the `list_head`
 * construct used in the Linux kernel, although very slimmed down for the sake
 * of simplicity. */

typedef struct List_Node List_Node;

struct List_Node {
	List_Node* next;
	List_Node* prev;
};

// Add list value after target
void list_add(List_Node* target, List_Node* new_node);

// Delete value from list by redirecting its neighbors
void list_add(List_Node* target, List_Node* new_node);

// Initialize node to be head of list
static inline
void list_init(List_Node* target){
	target->next = target;
	target->prev = target;
}

// Get pointer of structure containing the list
#define list_entry(Ptr, Type, Member) container_of(Ptr, Type, Member)

#define list_foreach(IterVar, ListHead) \
	for(List_Node* IterVar = ListHead.next; IterVar != &ListHead; IterVar = IterVar->next)

#define list_foreach_reversed(IterVar, ListHead) \
	for(List_Node* IterVar = ListHead.prev; IterVar != &ListHead; IterVar = IterVar->prev)

#ifdef BASE_C_IMPLEMENTATION

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

#endif

i32 hash_fnv32_ex(void const* data, isize len, i32 seed);

i64 hash_fnv64_ex(void const* data, isize len, i64 seed);

i32 hash_fnv32(void const* data, isize len);

i64 hash_fnv64(void const* data, isize len);

#ifdef BASE_C_IMPLEMENTATION
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

#endif

#include <stdatomic.h>

#define SPINLOCK_LOCKED 1
#define SPINLOCK_UNLOCKED 0

typedef struct {
	atomic_int _state;
} Spinlock;

void spinlock_acquire(Spinlock* l);

void spinlock_release(Spinlock* l);

#define SCOPED_SPINLOCK(LockPtr, Scope) \
	do { spinlock_acquire(LockPtr); do { Scope } while(0); spinlock_release(LockPtr); } while(0)

#ifdef BASE_C_IMPLEMENTATION

void spinlock_acquire(Spinlock* l){
	for(;;){
		if(!atomic_exchange_explicit(&l->_state, SPINLOCK_LOCKED, memory_order_acquire)){
			break;
		}
		/* Busy wait while locked */
		while(atomic_load_explicit(&l->_state, memory_order_relaxed));
	}
}

void spinlock_release(Spinlock* l){
	atomic_store(&l->_state, SPINLOCK_UNLOCKED);
}
#endif

#define New(T_, N_, Al_) mem_alloc((Al_), sizeof(T_) * (N_), alignof(T_))

typedef struct {
	byte* data;
	isize len;
} Bytes;

enum Allocator_Op {
	Mem_Op_Alloc    = 1,
	Mem_Op_Resize   = 2,
	Mem_Op_Free     = 3,
	Mem_Op_Free_All = 4,

	Mem_Op_Query = 0,
};

enum Allocator_Capability {
	Allocator_Alloc_Any = 1 << 0,
	Allocator_Free_Any  = 1 << 1,
	Allocator_Free_All  = 1 << 2,
	Allocator_Resize    = 1 << 3,
	Allocator_Align_Any = 1 << 4,
};

typedef void* (*Mem_Allocator_Func) (
	void* impl,
	enum Allocator_Op op,
	void* old_ptr,
	isize size, isize align,
	i32* capabilities
);

typedef struct {
	Mem_Allocator_Func func;
	void* data;
} Mem_Allocator;

static inline
void mem_set(void* p, byte val, isize nbytes){
	__builtin_memset(p, val, nbytes);
}

static inline
void mem_copy(void* dest, void const * src, isize nbytes){
	__builtin_memmove(dest, src, nbytes);
}

static inline
int mem_valid_alignment(isize align){
	return (align & (align - 1)) == 0 && (align != 0);
}

// Align p to alignment a, this only works if a is a non-zero power of 2
static inline
uintptr align_forward_ptr(uintptr p, uintptr a){
	debug_assert(mem_valid_alignment(a), "Invalid memory alignment");
	uintptr mod = p & (a - 1);
	if(mod > 0){
		p += (a - mod);
	}
	return p;
}

// Align p to alignment a, this works for any positive non-zero alignment
static inline
uintptr align_forward_size(isize p, isize a){
	debug_assert(a > 0, "Invalid size alignment");
	isize mod = p % a;
	if(mod > 0){
		p += (a - mod);
	}
	return p;
}

// Get capabilities of allocator as a number, you can use bit operators to check it.
i32 allocator_query_capabilites(Mem_Allocator allocator, i32* capabilities);

// Allocate fresh memory, filled with 0s. Returns NULL on failure.
void* mem_alloc(Mem_Allocator allocator, isize size, isize align);

// Re-allocate memory in-place without changing the original pointer. Returns NULL on failure.
void* mem_resize(Mem_Allocator allocator, void* ptr, isize new_size);

// Free pointer to memory, includes alignment information, which is required for
// some allocators, freeing NULL is a no-op
void mem_free_ex(Mem_Allocator allocator, void* p, isize align);

// Free pointer to memory, freeing NULL is a no-op
void mem_free(Mem_Allocator allocator, void* p);

// Free all pointers owned by allocator
void mem_free_all(Mem_Allocator allocator);

#ifdef BASE_C_IMPLEMENTATION

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

#endif

typedef enum {
	IO_Op_Query = 0,
	IO_Op_Read  = 1 << 0,
	IO_Op_Write = 1 << 1,
} IO_Operation;

typedef isize (*IO_Func)(void* impl, IO_Operation op, byte* data, isize len);

typedef struct {
	void* impl;
	IO_Func func;
} IO_Stream;

typedef struct { IO_Stream _stream; } IO_Writer;

typedef struct { IO_Stream _stream; } IO_Reader;

typedef enum {
	IO_Err_None = 0,
	IO_Err_End = -1,
	IO_Err_Closed = -2,
	IO_Err_Out_Of_Memory = -3,
	IO_Socket_Error = -4,
	IO_Err_Unknown = -127,
} IO_Error;

// Read into buffer, returns number of bytes read into buffer. On error returns
// the negative valued IO_Error code.
isize io_read(IO_Reader r, byte* buf, isize buflen);

// Write into buffer, returns number of bytes written to buffer. On error returns
// the negative valued IO_Error code.
isize io_write(IO_Writer r, byte const* buf, isize buflen);

// Query the capabilites of the IO stream as per IO_Operation
i8 io_query_stream(IO_Stream s);

// Cast a stream to a IO reader, in debug mode this uses a query to assert that
// the stream supports reading
IO_Reader io_to_reader(IO_Stream s);

// Cast a stream to a IO writer, in debug mode this uses a query to assert that
// the stream supports writing
IO_Writer io_to_writer(IO_Stream s);

#ifdef BASE_C_IMPLEMENTATION

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

#endif

// Helper to use with printf "%.*s"
#define FmtString(str_) (int)((str_).len), (str_).data

typedef struct {
	isize len;
	byte const * data;
} String;

static inline
isize cstring_len(cstring cstr){
	static const isize CSTR_MAX_LENGTH = (~(u32)0) >> 1;
	isize size = 0;
	for(isize i = 0; i < CSTR_MAX_LENGTH && cstr[i] != 0; i += 1){
		size += 1;
	}
	return size;
}

// Create substring from a cstring
String str_from(cstring data);

// Create substring from a piece of a cstring
String str_from_range(cstring data, isize start, isize length);

// Create substring from a raw slice of bytes
String str_from_bytes(byte const* data, isize length);

// Get a sub string, starting at `start` with `length`
String str_sub(String s, isize start, isize length);

// Get how many codeponits are in a string
isize str_codepoint_count(String s);

// Get the byte offset of the n-th codepoint
isize str_codepoint_offset(String s, isize n);

// Clone a string
String str_clone(String s, Mem_Allocator allocator);

// Destroy a string
void str_destroy(String s, Mem_Allocator allocator);

// Concatenate 2 strings
String str_concat(String a, String b, Mem_Allocator allocator);

// Check if 2 strings are equal
bool str_eq(String a, String b);

// Trim leading codepoints that belong to the cutset
String str_trim_leading(String s, String cutset);

// Trim trailing codepoints that belong to the cutset
String str_trim_trailing(String s, String cutset);

// Trim leading and trailing codepoints
String str_trim(String s, String cutset);

// Get an utf8 iterator from string
UTF8_Iterator str_iterator(String s);

// Get an utf8 iterator from string, already at the end, to be used for reverse iteration
UTF8_Iterator str_iterator_reversed(String s);

// Is string empty?
bool str_empty(String s);

#ifdef BASE_C_IMPLEMENTATION

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
String str_sub(String s, isize start, isize length){
	if(start >= s.len || start < 0 || start + length > s.len){ return EMPTY; }

	String sub = {
		.data = &s.data[start],
		.len = length,
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

#endif

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
bool buffer_write(Bytes_Buffer* bb, byte const* b, isize len);

// Current unread bytes, this pointer becomes invalid as soon as the buffer is modified.
byte* buffer_bytes(Bytes_Buffer* bb);

IO_Stream buffer_stream(Bytes_Buffer* bb);

#ifdef BASE_C_IMPLEMENTATION

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

#endif

typedef struct {
	isize offset;
	isize capacity;
	byte* data;
} Mem_Arena;

void arena_init(Mem_Arena* a, byte* data, isize len);
void arena_destroy(Mem_Arena *a);
Mem_Allocator arena_allocator(Mem_Arena* a);

#ifdef BASE_C_IMPLEMENTATION

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
#endif


Mem_Allocator heap_allocator();

#ifdef BASE_C_IMPLEMENTATION
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
#endif

// Reads whole file into memory, it allocates one extra byte implicitly and sets
// it to 0, to allow for better interop with cstrings.
Bytes file_read_all(String path, Mem_Allocator allocator);

// Write n bytes of data to file at path. Returns number of bytes written
// (negative means error).
isize file_write(String path, byte const* data, isize n);

// Append n bytes of data to file at path. Returns number of bytes added
// (negative means error).
isize file_append(String path, byte const* data, isize n);

#ifdef BASE_C_IMPLEMENTATION

#include <stdio.h>
#define MAX_PATH_LEN 4096

static inline
isize _file_add_content(cstring path, cstring flags, byte const* data, isize nbytes){
	FILE* f = fopen(path, flags);
	if(f == NULL){ return -1; }

	isize written = fwrite(data, 1, nbytes, f);
	fclose(f);
	return written;
}

isize file_write(String path, byte const* data, isize n){
	char path_buf[MAX_PATH_LEN] = {0};
	mem_copy(path_buf, path.data, min(path.len, MAX_PATH_LEN - 1));
	return _file_add_content(path_buf, "wb", data, n);
}

isize file_append(String path, byte const* data, isize n){
	char path_buf[MAX_PATH_LEN] = {0};
	mem_copy(path_buf, path.data, min(path.len, MAX_PATH_LEN - 1));
	return _file_add_content(path_buf, "ab", data, n);
}

Bytes file_read_all(String path, Mem_Allocator allocator){
	static const Bytes error = {0, 0};

	char path_buf[MAX_PATH_LEN] = {0};
	mem_copy(path_buf, path.data, min(path.len, MAX_PATH_LEN - 1));

	FILE* f = fopen(path_buf, "rb");
	if(f == NULL){ goto error_exit; }

	fseek(f, 0, SEEK_END);
	isize end = ftell(f);
	rewind(f);
	isize start = ftell(f);

	isize size = end - start;
	if(size <= 0){ return error; }

	byte* data = New(byte, size + 1, allocator);
	if(data == NULL){ goto error_exit; }
	data[size] = 0;

	size = fread(data, 1, size, f);
	fclose(f);
	return (Bytes){ .data = data, .len = size };

error_exit:
	if(f != NULL) { fclose(f); }
	return error;
}
#endif

typedef enum {
    Net_IPv4,
    Net_IPv6,
} Net_Address_Family;

typedef struct {
    Net_Address_Family family;
    union {
        u8 ip4[4] ;
        u8 ip6[16];
    } data;
} Net_Address;

#define IPV6_LOOPBACK_ADDR \
    (Net_Address){ \
        .family = Net_IPv6, \
        .data.ip6 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1} \
    }

#define IPV4_LOOPBACK_ADDR \
    (Net_Address){ \
        .family = Net_IPv4, \
        .data.ip4 = {127, 0, 0, 1} \
    }

// Big endian u16
typedef struct {
	u16 data;
} Net_Port;

typedef enum {
    Transport_TCP,
    Transport_UDP,
} Net_Transport_Protocol;

typedef struct {
    Net_Address address;
    Net_Port port;
} Net_Endpoint;

// Handle to an OS socket
typedef struct {
    i64 _handle;
} Net_Raw_Socket;

// Represents any socket, includes protocol information
typedef struct {
    i64 _handle;
    Net_Transport_Protocol proto;
} Net_Socket;

static const Net_Socket BAD_SOCKET = {._handle = -1};

// Returns if bind was successful
bool net_bind(Net_Socket sock, Net_Endpoint endpoint);

// Returns a BAD_SOCKET on error
Net_Socket net_create_socket(Net_Address_Family family, Net_Transport_Protocol proto);

// Send payload to endpoint using socket
isize net_send_udp(Net_Socket sock, Bytes payload, Net_Endpoint to);

// Receive payload from UDP socket, the remote address is written to `remote` if it is not NULL
isize net_receive_udp(Net_Socket sock, Bytes buf, Net_Endpoint* remote);

// Connect to endpoint using TCP socket
bool net_connect_tcp(Net_Socket sock, Net_Endpoint remote);

// Send payload that sock is connected to, returs number of bytes sent
isize net_send_tcp(Net_Socket sock, Bytes payload);

// Listen to TCP connections on socket
bool net_listen_tcp(Net_Socket sock);

// Close a socket
bool net_close_socket(Net_Socket sock);

static inline
bool net_socket_ok(Net_Socket s){
    return s._handle != BAD_SOCKET._handle;
}

#ifdef BASE_C_IMPLEMENTATION

#ifdef __linux__

#include <unistd.h>
#include <arpa/inet.h>

static inline
Net_Port net_port_from(u16 val){
	Net_Port np = {val};
	if(!arch_is_big_endian()){
		swap_bytes(&np);
	}
	return np;
}

static inline
u16 net_port_to_number(Net_Port p){
	if(!arch_is_big_endian()){
		swap_bytes(&p);
	}
	return p.data;
}

static inline
int _unwrap_addr_family(Net_Address_Family family){
    switch (family) {
    case Net_IPv4: return AF_INET;
    case Net_IPv6: return AF_INET6;
    default: return -1;
    }
}

static inline
int _unwrap_sock_protocol(Net_Transport_Protocol proto){
    switch (proto) {
    case Transport_TCP: return SOCK_STREAM;
    case Transport_UDP: return SOCK_DGRAM;
    default: return -1;
    }
}

static inline
struct sockaddr_in _unwrap_endpoint_ip4(Net_Endpoint addr){
	struct sockaddr_in os_addr = {0};

	os_addr.sin_family = _unwrap_addr_family(addr.address.family);
	os_addr.sin_port = addr.port.data;
	mem_copy(&os_addr.sin_addr.s_addr, addr.address.data.ip4, 4);

	return os_addr;
}

static inline
struct sockaddr_in6 _unwrap_endpoint_ip6(Net_Endpoint addr){
	struct sockaddr_in6 os_addr = {0};
	os_addr.sin6_port = addr.port.data;
	os_addr.sin6_family = _unwrap_addr_family(addr.address.family);
	mem_copy(&os_addr.sin6_addr, addr.address.data.ip6, 16);

	return os_addr;
}

bool net_bind(Net_Socket sock, Net_Endpoint endpoint){
	int status = -1;
	switch(endpoint.address.family){
		case Net_IPv4: {
			struct sockaddr_in os_endpoint = _unwrap_endpoint_ip4(endpoint);
			status = bind(sock._handle, (struct sockaddr const *)(&os_endpoint), sizeof(os_endpoint));
		} break;

		case Net_IPv6: {
			struct sockaddr_in6 os_endpoint = _unwrap_endpoint_ip6(endpoint);
			status = bind(sock._handle, (struct sockaddr const *)(&os_endpoint), sizeof(os_endpoint));
		} break;
	}

	return status >= 0;
}

#define NET_TCP_LISTEN_COUNT 8

bool net_listen_tcp(Net_Socket sock){
	debug_assert(sock.proto == Transport_TCP, "Wrong transport protocol");
	int status = listen(sock._handle, NET_TCP_LISTEN_COUNT);
	return status == 0;
}

Net_Socket net_accept_tcp(Net_Socket sock, Net_Endpoint* end_in){
	debug_assert(sock.proto == Transport_TCP, "Wrong transport protocol");

#define ADDR_LEN (sizeof(struct sockaddr) * (isize)(4))
	socklen_t addr_len = ADDR_LEN;
	alignas(alignof(struct sockaddr)) byte addr_data[ADDR_LEN] = {0}; // Enough to store IPv6
#undef ADDR_LEN

	int newsock = accept(sock._handle, (struct sockaddr*)&addr_data, &addr_len);
	if(newsock < 0){
		return BAD_SOCKET;
	}
	printf("OK!\n");
	return (Net_Socket){ ._handle = newsock, .proto = Transport_TCP };
}

bool net_connect_tcp(Net_Socket sock, Net_Endpoint remote){
	debug_assert(sock.proto == Transport_TCP, "Wrong transport protocol");
	int status = 0;
	switch(remote.address.family){
		case Net_IPv4: {
			struct sockaddr_in os_endpoint = _unwrap_endpoint_ip4(remote);
			status = connect(sock._handle, (struct sockaddr const *)(&os_endpoint), sizeof(os_endpoint));
		} break;
		case Net_IPv6: {
			struct sockaddr_in6 os_endpoint = _unwrap_endpoint_ip6(remote);
			status = connect(sock._handle, (struct sockaddr const *)(&os_endpoint), sizeof(os_endpoint));
		} break;
	}

	return status >= 0;
}

isize net_send_tcp(Net_Socket sock, Bytes payload){
	debug_assert(sock.proto == Transport_TCP, "Wrong transport protocol");
	isize n = send(sock._handle, payload.data, payload.len, 0);
	return n;
}

isize net_send_udp(Net_Socket sock, Bytes payload, Net_Endpoint to){
	debug_assert(sock.proto == Transport_UDP, "Wrong transport protocol");
	switch(to.address.family){
		case Net_IPv4: {
			struct sockaddr_in os_endpoint = _unwrap_endpoint_ip4(to);
			isize n = sendto(
				sock._handle,
				payload.data,
				payload.len,
				0,
				(struct sockaddr const *)(&os_endpoint), sizeof(os_endpoint));
			return n;
		} break;

		case Net_IPv6: {
			struct sockaddr_in6 os_endpoint = _unwrap_endpoint_ip6(to);
			isize n = sendto(
				sock._handle,
				payload.data,
				payload.len,
				0,
				(struct sockaddr const *)(&os_endpoint), sizeof(os_endpoint));
			return n;
		} break;
	}

	return -1;
}

isize net_receive_tcp(Net_Socket sock, Bytes buf){
	int n = recv(sock._handle, buf.data, buf.len, 0);
	return n;
}

isize net_receive_udp(Net_Socket sock, Bytes buf, Net_Endpoint* remote){
	debug_assert(sock.proto == Transport_UDP, "Wrong transport protocol");
	struct sockaddr_storage addr_data = {0};
	uint addr_len = 64;
		printf("b\n");
	isize n = recvfrom(sock._handle, buf.data, buf.len, 0, (struct sockaddr*)&addr_data, &addr_len);
		printf("c\n");

	if(remote){
		if(addr_len == sizeof(struct sockaddr_in)){
			struct sockaddr_in os_addr = *(struct sockaddr_in *)(&addr_data);
			// if(!arch_is_big_endian()){
			// 	swap_bytes(&os_addr.sin_addr.s_addr);
			// 	swap_bytes(&os_addr.sin_port);
			// }

			remote->address.family = Net_IPv4;
			remote->port.data = os_addr.sin_port;
			mem_copy(&remote->address.data.ip4, &os_addr.sin_addr, 4);
		}
		else if(addr_len == sizeof(struct sockaddr_in6)){
			struct sockaddr_in6 os_addr = *(struct sockaddr_in6 *)(&addr_data);
			// if(!arch_is_big_endian()){
			// 	swap_bytes(&os_addr.sin6_addr);
			// 	swap_bytes(&os_addr.sin6_port);
			// }

			remote->address.family = Net_IPv6;
			remote->port.data = os_addr.sin6_port;
			mem_copy(&remote->address.data.ip6, &os_addr.sin6_addr, 16);
		}
	}

	return n;
}

Net_Socket net_create_socket(Net_Address_Family family, Net_Transport_Protocol proto){
    int af = _unwrap_addr_family(family);
    int sp = _unwrap_sock_protocol(proto);

    int sock_fd = socket(af, sp, 0);
    if(sock_fd < 0){
        return BAD_SOCKET;
    }
    // TODO set REUSE flag
	int option = 1;
	int opt_status = setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(int));

	if(opt_status < 0){
		close(sock_fd);
		return BAD_SOCKET;
	}

    return (Net_Socket){
        .proto = proto,
        ._handle = sock_fd,
    };
}

bool net_close_socket(Net_Socket sock){
	int status = close(sock._handle);
	return status >= 0;
}

#else
#error "Unimplemented platform"
#endif

#endif
