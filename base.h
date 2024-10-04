#pragma once
/* Essential definitions. */

#define BASE_C_VERSION "592aa33a07a0ba8654d6d6be75791e271669ac97"

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

// Hash data using FNV-1a hash (32-bit)
i32 hash_fnv32_ex(void const* data, isize len, i32 seed);

// Hash data using FNV-1a hash (64-bit)
i64 hash_fnv64_ex(void const* data, isize len, i64 seed);

// Hash data using FNV-1a hash (32-bit)
i32 hash_fnv32(void const* data, isize len);

// Hash data using FNV-1a hash (64-bit)
i64 hash_fnv64(void const* data, isize len);
#include <stdatomic.h>

#define SPINLOCK_LOCKED 1
#define SPINLOCK_UNLOCKED 0

// The zeroed state of a spinlock is unlocked, to be effective across threads
// it's important to keep the spinlock outside of the stack and never mark it as
// a thread_local struct.
typedef struct {
	atomic_int _state;
} Spinlock;

// Enter a busy wait loop until spinlock is acquired(locked)
void spinlock_acquire(Spinlock* l);

// Try to lock spinlock, if failed, just move on. Returns if lock was locked.
bool spinlock_try_acquire(Spinlock* l);

// Release(unlock) the spinlock
void spinlock_release(Spinlock* l);

#define spinlock_scoped(LockPtr, Scope) \
	do { spinlock_acquire(LockPtr); do { Scope } while(0); spinlock_release(LockPtr); } while(0)

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

// Set n bytes of p to value.
void mem_set(void* p, byte val, isize nbytes);

// Copy n bytes for source to destination, they may overlap.
void mem_copy(void* dest, void const * src, isize nbytes);

// Copy n bytes for source to destination, they should not overlap, this tends
// to be faster then mem_copy
void mem_copy_no_overlap(void* dest, void const * src, isize nbytes);

// Align p to alignment a, this only works if a is a non-zero power of 2
uintptr align_forward_ptr(uintptr p, uintptr a);

// Align p to alignment a, this works for any positive non-zero alignment
uintptr align_forward_size(isize p, isize a);

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

static inline
bool mem_valid_alignment(isize align){
	return (align & (align - 1)) == 0 && (align != 0);
}

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

// TODO: Use _Generic for better ergonomics

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

typedef enum {
	CLI_Flag_None = 0,
	CLI_Flag_Toggle = 1,
	CLI_Flag_Key_Value = 2,
} CLI_Flag_Type;

typedef struct {
	String value;
	String key;
	CLI_Flag_Type type;
} CLI_Arg;

// Parse a CLI argument, accepts flags of the form `-key:val` or just `-flag`
CLI_Arg cli_parse_arg(String arg);

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

typedef struct {
	isize offset;
	isize capacity;
	byte* data;
} Mem_Arena;

// Initialize a memory arena with a buffer
void arena_init(Mem_Arena* a, byte* data, isize len);

// Deinit the arena
void arena_destroy(Mem_Arena *a);

// Get arena as a conforming instance to the allocator interface
Mem_Allocator arena_allocator(Mem_Arena* a);

// Get the system's heap allocator, this will typically be LibC's but could also
// be a custom one or a 3rd party one like mimalloc.
Mem_Allocator heap_allocator();

