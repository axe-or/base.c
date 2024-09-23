#include "prelude.h"
#define BASE_C_IMPLEMENTATION 1
#include "string.h"
#include "net.h"
#include <stdio.h>

int main(){
	printf("Making sock\n");
	Net_Socket sk = net_create_socket(Net_IPv6, Transport_UDP);
	if(!net_socket_ok(sk)){
		panic("Failed to create socket");
	}
	printf("Made sock\n");

	cstring msg = "Hello";

	Net_Endpoint server = {0};
	server.address = (Net_Address) {
	  .family = Net_IPv6, .data.ip6 = { 0 },
	};
	server.port = 9000;

	printf("Binding sock\n");
	if(!net_bind(sk, server)){
		panic("Failed to bind to address");
	}
	printf("Sock bound\n");

	static byte buf[4096] = {0};
	Bytes buffer = {buf, 4096};
	Net_Endpoint e = {
		.address = IPV6_LOOPBACK_ADDR,
		.port = 6969,
	};

	while(1){
		net_receive_udp(net_udp_sock(sk), buffer, &e);
		printf(" %.*s\n", FmtString(buffer));
	}
	return 0;
}
