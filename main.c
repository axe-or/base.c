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
		.address = IPV4_LOOPBACK_ADDR,
		.port = 9000
	};

	if(!net_listen_tcp(sock)){
		panic("Could not listen TCP");
	}

	while(1){
		Net_Socket conn = net_accept_tcp(sock, NULL);
	}

	return 0;
}
