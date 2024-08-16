#include "utf8.h"
#include "testing.h"
#include "string.h"

static inline void print_bytes(byte const *b, isize n){
	for(isize i = 0; i < n; i += 1){
		printf("%02x ", b[i]);
	}
	printf("\n");
}

Test_Func test_utf8(){
	Test_Begin("UTF-8");
	const Codepoint codepoints[] = {
		0x0024,  /* $ */
		0x0418,  /* И */
		0xd55c,  /* 한 */
		0x10348, /* 𐍈 */
	};
	const cstring encoded[] = {"$", "И", "한", "𐍈"};

	isize const N = (sizeof(codepoints) / sizeof(Codepoint));

	for(isize i = 0; i < N; i += 1){
		UTF8_Encode_Result res = utf8_encode(codepoints[i]);
		Expect(res.len == i + 1);

		String a = str_from_bytes(res.bytes, res.len);
		String b = str_from(encoded[i]);

		Expect(str_eq(a, b));
		// printf("(%06x:%d) > %.*s\n", codepoints[i], res.len, res.len, res.bytes);
	}

	for(isize i = 0; i < N; i += 1){
		UTF8_Decode_Result res = utf8_decode((byte const *)encoded[i], cstring_len(encoded[i]));
		Expect(res.len == i + 1);

		Codepoint a = codepoints[i];
		Codepoint b = res.codepoint;
		Expect(a == b);
	}

	Test_End();
}

