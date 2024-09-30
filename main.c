#define BASE_C_IMPLEMENTATION 1
#include "prelude.h"
#include "heap_allocator.h"
#include "string.h"
#include "net.h"
#include <stdio.h>

#define container_name Int_Array
#define container_type_param i64
#define container_prefix ia
#include "generic/dynamic_array.h"

int main(){
	Net_Socket sock = net_create_socket(Net_IPv4, Transport_UDP);
	if(!net_socket_ok(sock)){
		printf("Failed to create socket");
	}

	Net_Endpoint server = {
		.address = {.family = Net_IPv4, .data.ip4 = {127,0,0,1}},
		.port = net_port_from(9000),
	};

	if(!net_bind(sock, server)){
		panic("Failed to bind listener");
	}

	while(1){
		Bytes buf = {.data = (byte[4096]){0}, .len = 4096};
		printf("a\n");
		isize n = net_receive_udp(sock, buf, NULL);
		printf(">>> %.*s\n", (int)n, buf.data);
		fflush(stdout);
	}

	// if(!net_listen_tcp(sock)){
	// 	panic("Could not listen TCP");
	// }

	// while(1){
	// 	printf("Awaiting connection...\n");
	// 	Bytes buf = {.data = (byte[4096]){0}, .len = 4096};
	// 	Net_Socket conn = net_accept_tcp(sock, NULL);
	// }

	return 0;
}
