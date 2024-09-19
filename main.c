#include "prelude.h"
#define BASE_C_IMPLEMENTATION 1
#include "string.h"
#include "net.h"
#include <stdio.h>

int main(){
	printf("Big endian: %d\n", arch_is_big_endian());
	i32 n = 0xfe;
	i32 ns = n;
	SwapBytes(&ns);
	printf("%08x\n", n);
	printf("%08x\n", ns);

	Net_Socket sk = net_create_socket(Net_IPv6, Transport_UDP);
	if(!net_socket_ok(sk)){
		panic("Failed to create socket");
	}
	printf("Socket %ld\n", sk._handle);

	cstring msg = "Hello";

	Net_Endpoint server = {0};
	server.address = (Net_Address) {
	  .family = Net_IPv6, .data.ip6 = { 0 },
	};
	server.port = 9000;

	isize res = net_send_udp(net_udp_sock(sk), (Bytes){.data = (void*)msg, .len = 5}, server);
	printf("Res: %lu\n", res);

	return 0;
}
