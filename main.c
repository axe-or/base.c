#define BASE_C_IMPLEMENTATION 1
#include "prelude.h"
#include "heap_allocator.h"
#include "net.h"
#include <stdio.h>

#define container_name Int_Array
#define container_type_param i64
#define container_prefix ia
#include "generic/dynamic_array.h"

int main(){
	Net_Socket sock = net_create_socket(Net_IPv6, Transport_TCP);
	Net_Endpoint remote = {
		.address = {.data = {0}, .family = Net_IPv6},
		.port = 9000
	};

	if(!net_connect_tcp(net_tcp_sock(sock), remote)){
		panic("Could not connect to TCP endpoint");
	}
	Bytes msg = {
		.data = (byte*)"Hello",
		.len = 5,
	};

	net_send_tcp(net_tcp_sock(sock), msg);

	return 0;
}
