#include "prelude.h"
#define BASE_C_IMPLEMENTATION 1
#include "string.h"
#include "net.h"
#include <stdio.h>

int main(){
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

	if(!net_bind(sk, server)){
		panic("Failed to bind to address");
	}

	return 0;
}
