#include "testing.h"

#include "arena_allocator.h"

Test_Func test_arena(){
	Test_Begin("Arena Allocator");
	static byte arena_mem[128];
	Mem_Arena arena;
	arena_init(&arena, arena_mem, 128);

	Mem_Allocator allocator = arena_allocator(&arena);
	Expect(arena.capacity == 128 && arena.data == arena_mem && arena.offset == 0);

	byte* buf = New(byte, 3, allocator);
	Expect(arena.offset == 3 && buf != NULL);

	i32* n = New(*n, 2, allocator);
	Expect(arena.offset == 3 + 8 + 1 && n != NULL);

	u8* buf2 = New(u8, arena.capacity - arena.offset, allocator);
	Expect(arena.offset == 128 && buf2 != NULL);

	u8* nil = New(u8, 1, allocator);
	Expect(arena.offset == 128 && nil == NULL);

	mem_free_all(allocator);
	Expect(arena.capacity == 128 && arena.data == arena_mem && arena.offset == 0);
	Test_End();
}
