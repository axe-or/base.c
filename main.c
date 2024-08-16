#define LIBC2_IMPLEMENTATION
#include "tests/bytes_buffer_test.c"
#include "tests/utf8_test.c"
#include "tests/arena_test.c"

int main(){
	// Run all tests
	test_bytes_buffer();
	test_utf8();
	test_arena();
}
