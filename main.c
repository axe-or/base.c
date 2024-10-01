#define _POSIX_C_SOURCE 199309L

#define BASE_C_IMPLEMENTATION 1
#include "prelude.h"
#include "heap_allocator.h"
#include "string.h"
#include "net.h"
#include "chrono.h"
#include <stdio.h>

#define container_name Int_Array
#define container_type_param i64
#define container_prefix ia
#include "generic/dynamic_array.h"

int main(){
	printf("A...");
	fflush(stdout);
	time_sleep(100000 * Time_Microsecond);
	printf("B!\n");
	return 0;
}
