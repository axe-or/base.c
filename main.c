#include "prelude.h"
#define BASE_C_IMPLEMENTATION 1
#include "string.h"
#include "net.h"
#include <stdio.h>

int main(){
	printf("Making sock\n");
	Net_Socket sk = net_create_socket(Net_IPv4, Transport_UDP);
	if(!net_socket_ok(sk)){
		panic("Failed to create socket");
	}
	printf("Made sock\n");

	cstring msg = "Hello";

	Net_Endpoint server = {0};
	server.address = (Net_Address) {
	  .family = Net_IPv4, .data.ip4 = { 0 },
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
		.address = IPV4_LOOPBACK_ADDR,
		.port = 6969,
	};

	while(1){
		printf("Hi\n");
		net_receive_udp(net_udp_sock(sk), buffer, &e);
		printf("Remote: IPv%d %08x ...:%d",
		e.address.family == Net_IPv4 ? 4 : 6, *(u32*)&e.address.data.ip4, e.port);
		if(buffer.data[0] == 'Q'){
			break;
		}
	}
	return 0;
}
