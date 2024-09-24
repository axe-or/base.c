#include "prelude.h"
#define BASE_C_IMPLEMENTATION 1
#include "string.h"
#include "spinlock.h"
#include <stdio.h>


static int counter = 0;


int increment(){}

int main(){
	static Spinlock sl = {0};

	return 0;
}
