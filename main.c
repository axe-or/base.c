#include "prelude.h"
#define BASE_C_IMPLEMENTATION 1
#include "string.h"
#include "net.h"
#include <stdio.h>

int main(){
	Net_Address addr = IPV6_LOOPBACK_ADDR;

	printf("Big endian: %d\n", arch_is_big_endian());
	i32 n = 0xfe;
	i32 ns = n;
	SwapBytes(&ns);
	printf("%08x\n", n);
	printf("%08x\n", ns);

	Net_Socket sk = net_create_socket(Net_IPv4, Transport_UDP);
	if(!net_socket_ok(sk)){
		panic("Failed to create socket");
	}

	return 0;
}
