#define BASE_C_IMPLEMENTATION 1
#include "base.h"
#include <stdio.h>

int main(){
	String s = str_from("  И И\t한\nHello.");
	String s2 = str_strip_leading(s, str_from(" 한И\t\n\r"));

	printf("\"%.*s\"\n", FmtString(s));
	printf("\"%.*s\"\n", FmtString(s2));

	return 0;
}
