#pragma once

#include "assert.h"
#include "memory.h"
#include "prelude.h"

typedef enum {
    Net_IPv4,
    Net_IPv6,
} Net_Address_Family;

typedef struct {
    Net_Address_Family family;
    union {
        u8 ip4[4] ;
        u8 ip6[16];
    } data;
} Net_Address;

#define IPV6_LOOPBACK_ADDR \
    (Net_Address){ \
        .family = Net_IPv6, \
        .data.ip6 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1} \
    }

#define IPV4_LOOPBACK_ADDR \
    (Net_Address){ \
        .family = Net_IPv4, \
        .data.ip4 = {127, 0, 0, 1} \
    }

// Big endian u16
typedef struct {
	u16 data;
} Net_Port;

typedef enum {
    Transport_TCP,
    Transport_UDP,
} Net_Transport_Protocol;

typedef struct {
    Net_Address address;
    Net_Port port;
} Net_Endpoint;

// Handle to an OS socket
typedef struct {
    i64 _handle;
} Net_Raw_Socket;

// Represents any socket, includes protocol information
typedef struct {
    i64 _handle;
    Net_Transport_Protocol proto;
} Net_Socket; 

static const Net_Socket BAD_SOCKET = {._handle = -1};

// Returns if bind was successful
bool net_bind(Net_Socket sock, Net_Endpoint endpoint);

// Returns a BAD_SOCKET on error
Net_Socket net_create_socket(Net_Address_Family family, Net_Transport_Protocol proto);

// Send payload to endpoint using socket
isize net_send_udp(Net_Socket sock, Bytes payload, Net_Endpoint to);

// Receive payload from UDP socket, the remote address is written to `remote` if it is not NULL
isize net_receive_udp(Net_Socket sock, Bytes buf, Net_Endpoint* remote);

// Connect to endpoint using TCP socket
bool net_connect_tcp(Net_Socket sock, Net_Endpoint remote);

// Send payload that sock is connected to, returs number of bytes sent
isize net_send_tcp(Net_Socket sock, Bytes payload);

// Listen to TCP connections on socket
bool net_listen_tcp(Net_Socket sock);

// Close a socket
bool net_close_socket(Net_Socket sock);

static inline
bool net_socket_ok(Net_Socket s){
    return s._handle != BAD_SOCKET._handle;
}

#ifdef BASE_C_IMPLEMENTATION

#ifdef __linux__

#include <unistd.h>
#include <arpa/inet.h>

static inline
Net_Port net_port_from(u16 val){
	Net_Port np = {val};
	if(!arch_is_big_endian()){
		swap_bytes(&np);
	}
	return np;
}

static inline
u16 net_port_to_number(Net_Port p){
	if(!arch_is_big_endian()){
		swap_bytes(&p);
	}
	return p.data;
}

static inline
int _unwrap_addr_family(Net_Address_Family family){
    switch (family) {
    case Net_IPv4: return AF_INET;
    case Net_IPv6: return AF_INET6;
    default: return -1;
    }
}

static inline
int _unwrap_sock_protocol(Net_Transport_Protocol proto){
    switch (proto) {
    case Transport_TCP: return SOCK_STREAM;
    case Transport_UDP: return SOCK_DGRAM;
    default: return -1;
    }
}

static inline
struct sockaddr_in _unwrap_endpoint_ip4(Net_Endpoint addr){
	struct sockaddr_in os_addr = {0};

	os_addr.sin_family = _unwrap_addr_family(addr.address.family);
	os_addr.sin_port = addr.port.data;
	mem_copy(&os_addr.sin_addr.s_addr, addr.address.data.ip4, 4);

	return os_addr;
}

static inline
struct sockaddr_in6 _unwrap_endpoint_ip6(Net_Endpoint addr){
	struct sockaddr_in6 os_addr = {0};
	os_addr.sin6_port = addr.port.data;
	os_addr.sin6_family = _unwrap_addr_family(addr.address.family);
	mem_copy(&os_addr.sin6_addr, addr.address.data.ip6, 16);

	return os_addr;
}

bool net_bind(Net_Socket sock, Net_Endpoint endpoint){
	int status = -1;
	switch(endpoint.address.family){
		case Net_IPv4: {
			struct sockaddr_in os_endpoint = _unwrap_endpoint_ip4(endpoint);
			status = bind(sock._handle, (struct sockaddr const *)(&os_endpoint), sizeof(os_endpoint));
		} break;

		case Net_IPv6: {
			struct sockaddr_in6 os_endpoint = _unwrap_endpoint_ip6(endpoint);
			status = bind(sock._handle, (struct sockaddr const *)(&os_endpoint), sizeof(os_endpoint));
		} break;
	}

	return status >= 0;
}

#define NET_TCP_LISTEN_COUNT 8

bool net_listen_tcp(Net_Socket sock){
	debug_assert(sock.proto == Transport_TCP, "Wrong transport protocol");
	int status = listen(sock._handle, NET_TCP_LISTEN_COUNT);
	return status == 0;
}

Net_Socket net_accept_tcp(Net_Socket sock, Net_Endpoint* end_in){
	debug_assert(sock.proto == Transport_TCP, "Wrong transport protocol");

#define ADDR_LEN (sizeof(struct sockaddr) * (isize)(4))
	socklen_t addr_len = ADDR_LEN;
	alignas(alignof(struct sockaddr)) byte addr_data[ADDR_LEN] = {0}; // Enough to store IPv6
#undef ADDR_LEN

	int newsock = accept(sock._handle, (struct sockaddr*)&addr_data, &addr_len);
	if(newsock < 0){
		return BAD_SOCKET;
	}
	printf("OK!\n");
	return (Net_Socket){ ._handle = newsock, .proto = Transport_TCP };
}

bool net_connect_tcp(Net_Socket sock, Net_Endpoint remote){
	debug_assert(sock.proto == Transport_TCP, "Wrong transport protocol");
	int status = 0;
	switch(remote.address.family){
		case Net_IPv4: {
			struct sockaddr_in os_endpoint = _unwrap_endpoint_ip4(remote);
			status = connect(sock._handle, (struct sockaddr const *)(&os_endpoint), sizeof(os_endpoint));
		} break;
		case Net_IPv6: {
			struct sockaddr_in6 os_endpoint = _unwrap_endpoint_ip6(remote);
			status = connect(sock._handle, (struct sockaddr const *)(&os_endpoint), sizeof(os_endpoint));
		} break;
	}

	return status >= 0;
}

isize net_send_tcp(Net_Socket sock, Bytes payload){
	debug_assert(sock.proto == Transport_TCP, "Wrong transport protocol");
	isize n = send(sock._handle, payload.data, payload.len, 0);
	return n;
}

isize net_send_udp(Net_Socket sock, Bytes payload, Net_Endpoint to){
	debug_assert(sock.proto == Transport_UDP, "Wrong transport protocol");
	switch(to.address.family){
		case Net_IPv4: {
			struct sockaddr_in os_endpoint = _unwrap_endpoint_ip4(to);
			isize n = sendto(
				sock._handle,
				payload.data,
				payload.len,
				0,
				(struct sockaddr const *)(&os_endpoint), sizeof(os_endpoint));
			return n;
		} break;

		case Net_IPv6: {
			struct sockaddr_in6 os_endpoint = _unwrap_endpoint_ip6(to);
			isize n = sendto(
				sock._handle,
				payload.data,
				payload.len,
				0,
				(struct sockaddr const *)(&os_endpoint), sizeof(os_endpoint));
			return n;
		} break;
	}

	return -1;
}

isize net_receive_tcp(Net_Socket sock, Bytes buf){
	int n = recv(sock._handle, buf.data, buf.len, 0);
	return n;
}

isize net_receive_udp(Net_Socket sock, Bytes buf, Net_Endpoint* remote){
	debug_assert(sock.proto == Transport_UDP, "Wrong transport protocol");
	struct sockaddr_storage addr_data = {0};
	uint addr_len = 64;
		printf("b\n");
	isize n = recvfrom(sock._handle, buf.data, buf.len, 0, (struct sockaddr*)&addr_data, &addr_len);
		printf("c\n");

	if(remote){
		if(addr_len == sizeof(struct sockaddr_in)){
			struct sockaddr_in os_addr = *(struct sockaddr_in *)(&addr_data);
			// if(!arch_is_big_endian()){
			// 	swap_bytes(&os_addr.sin_addr.s_addr);
			// 	swap_bytes(&os_addr.sin_port);
			// }

			remote->address.family = Net_IPv4;
			remote->port.data = os_addr.sin_port;
			mem_copy(&remote->address.data.ip4, &os_addr.sin_addr, 4);
		}
		else if(addr_len == sizeof(struct sockaddr_in6)){
			struct sockaddr_in6 os_addr = *(struct sockaddr_in6 *)(&addr_data);
			// if(!arch_is_big_endian()){
			// 	swap_bytes(&os_addr.sin6_addr);
			// 	swap_bytes(&os_addr.sin6_port);
			// }

			remote->address.family = Net_IPv6;
			remote->port.data = os_addr.sin6_port;
			mem_copy(&remote->address.data.ip6, &os_addr.sin6_addr, 16);
		}
	}

	return n;
}

Net_Socket net_create_socket(Net_Address_Family family, Net_Transport_Protocol proto){
    int af = _unwrap_addr_family(family); 
    int sp = _unwrap_sock_protocol(proto);

    int sock_fd = socket(af, sp, 0);
    if(sock_fd < 0){
        return BAD_SOCKET;
    }
    // TODO set REUSE flag
	int option = 1;
	int opt_status = setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(int));

	if(opt_status < 0){
		close(sock_fd);
		return BAD_SOCKET;
	}

    return (Net_Socket){
        .proto = proto,
        ._handle = sock_fd,
    };
}

bool net_close_socket(Net_Socket sock){
	int status = close(sock._handle);
	return status >= 0;
}

#else
#error "Unimplemented platform"
#endif


#endif
