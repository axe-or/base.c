#define BASE_C_IMPLEMENTATION 1
#include "string.h"
#include <stdio.h>

int main(){
	String s = str_from("  И И\t한Hello.한\n \tИ");
	String s2 = str_trim(s, str_from(" 한И\t\n\r"));
	printf("\"%.*s\"\n", FmtString(s));
	printf("\"%.*s\"\n", FmtString(s2));

	return 0;
}
