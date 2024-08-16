#include "testing.h"
#include "string.h"
#include "heap_allocator.h"
#include "bytes_buffer.h"

Test_Func test_bytes_buffer(){
	Test_Begin("Bytes Buffer");
	Bytes_Buffer bb;
	buffer_init(&bb, heap_allocator(), 64);
	Expect(bb.cap == 64 && bb.len == 0 && bb.data != NULL);

	String msg1 = str_from("Hello, World!");
	Expect(buffer_write(&bb, msg1.data, msg1.len));

	Expect(str_eq(msg1, str_from_bytes(buffer_bytes(&bb), bb.len)));

	String msg2 = str_from(" It's the builder");
	buffer_write(&bb, msg2.data, msg2.len);

	String res = str_concat(msg1, msg2, heap_allocator());
	Expect(str_eq(res, str_from_bytes(buffer_bytes(&bb), bb.len)));

	static byte discard[256];

	{
		isize x = buffer_read(&bb, discard, msg1.len);
		Expect(x == msg1.len);
	}
	res = str_from_bytes(buffer_bytes(&bb), bb.len);
	Expect(str_eq(msg2, res));
	// printf("[%ld] %*.s\n", res.len, (int)res.len, res.data);
	{
		isize x = buffer_read(&bb, discard, msg2.len);
		Expect(x == msg2.len);
	}
	res = str_from_bytes(buffer_bytes(&bb), bb.len);
	Expect(res.len == 0);


	String s = str_from("Still here!");
	Expect(buffer_write(&bb, s.data, s.len));
	Expect(str_eq(s, str_from_bytes(buffer_bytes(&bb), bb.len)));
	buffer_clean_read_bytes(&bb);
	Expect(str_eq(s, str_from_bytes(buffer_bytes(&bb), bb.len)));
	// printf("[%ld] %*.s\n", t.len, (int)t.len, t.data);

	{
		byte* big = mem_alloc(heap_allocator(), 123, 1);
		buffer_clean_read_bytes(&bb);
		isize oldcap = bb.cap;
		Expect(buffer_write(&bb, big, 123));
		Expect(bb.cap > oldcap);
	}

	buffer_destroy(&bb);
	Test_End();
}
