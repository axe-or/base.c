#define BASE_C_IMPLEMENTATION 1
#include "base.h"
#include <stdio.h>

int main(){
	String s = str_from("  И И\t한Hello.한");
	String s2 = str_strip_leading(s, str_from(" 한И\t\n\r"));
	printf("\"%.*s\"\n", FmtString(s));
	// printf("\"%.*s\"\n", FmtString(s2));

	UTF8_Iterator iter = {
		.data = s.data,
		.data_length = s.len,
		.current = s.len,
	};

	Codepoint c; i8 n;
	while(utf8_iter_prev(&iter, &c, &n)){
		printf("U+%04x %d\n", c, n);
	}

	return 0;
}
