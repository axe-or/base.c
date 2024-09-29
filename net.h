#pragma once

#include "assert.h"
#include "io.h"
#include "memory.h"
#include "prelude.h"
#include <netinet/in.h>
#include <sys/socket.h>

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

typedef enum {
    Transport_TCP,
    Transport_UDP,
} Net_Transport_Protocol;

typedef struct {
    Net_Address address;
    u16 port;
} Net_Endpoint;

// Handle to an OS socket
typedef struct {
    i64 _handle;
} Net_Raw_Socket;

// Represents any socket, includes protocol information
typedef struct {
    Net_Transport_Protocol proto;
    i64 _handle;
} Net_Socket; 

// TCP Socket
typedef struct { i64 _handle; } Net_TCP_Socket;

// UDP Socket
typedef struct { i64 _handle; } Net_UDP_Socket;

static const Net_Socket BAD_SOCKET = {._handle = -1};

// Returns if bind was successful
bool net_bind(Net_Socket sock, Net_Endpoint endpoint);

// Returns a BAD_SOCKET on error
Net_Socket net_create_socket(Net_Address_Family family, Net_Transport_Protocol proto);

// Send payload to endpoint using socket
isize net_send_udp(Net_UDP_Socket sock, Bytes payload, Net_Endpoint to);

// Receive payload from UDP socket, the remote address is written to `remote` if it is not NULL
isize net_receive_udp(Net_UDP_Socket sock, Bytes buf, Net_Endpoint* remote);

// Cast a generic socket to a UDP socket.
static inline
Net_UDP_Socket net_udp_sock(Net_Socket sock){
	bool ok = sock.proto == Transport_UDP;
	debug_assert(ok, "Not a UDP socket.");
	return (Net_UDP_Socket){
		._handle = ok ? sock._handle : 0,
	};
}

// Cast a generic socket to a TCP socket.
static inline
Net_TCP_Socket net_tcp_sock(Net_Socket sock){
	bool ok = sock.proto == Transport_TCP;
	debug_assert(ok, "Not a TCP socket.");
	return (Net_TCP_Socket){
		._handle = ok ? sock._handle : 0,
	};
}


static inline
bool net_socket_ok(Net_Socket s){
    return s._handle != BAD_SOCKET._handle;
}

#ifdef BASE_C_IMPLEMENTATION

#ifdef __linux__

#include <unistd.h>
#include <arpa/inet.h>

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
	os_addr.sin_port = addr.port;
	mem_copy(&os_addr.sin_addr.s_addr, addr.address.data.ip4, 4);
	if(!arch_is_big_endian()){
		swap_bytes(&os_addr.sin_addr.s_addr);
		swap_bytes(&os_addr.sin_port);
	}
	return os_addr;
}

static inline
struct sockaddr_in6 _unwrap_endpoint_ip6(Net_Endpoint addr){
	struct sockaddr_in6 os_addr = {0};
	os_addr.sin6_port = addr.port;
	os_addr.sin6_family = _unwrap_addr_family(addr.address.family);
	mem_copy(&os_addr.sin6_addr, addr.address.data.ip6, 16);

	if(!arch_is_big_endian()){
		swap_bytes(&os_addr.sin6_addr);
		swap_bytes(&os_addr.sin6_port);
	}
	return os_addr;
}


bool net_bind(Net_Socket sock, Net_Endpoint endpoint){
	switch(endpoint.address.family){
		case Net_IPv4: {
			struct sockaddr_in os_endpoint = _unwrap_endpoint_ip4(endpoint);
			int status = bind(sock._handle, (struct sockaddr const *)(&os_endpoint), sizeof(os_endpoint));
			return status >= 0;
		} break;

		case Net_IPv6: {
			struct sockaddr_in6 os_endpoint = _unwrap_endpoint_ip6(endpoint);
			int status = bind(sock._handle, (struct sockaddr const *)(&os_endpoint), sizeof(os_endpoint));
			return status >= 0;
		} break;
	}

	return false;
}

isize net_send_udp(Net_UDP_Socket sock, Bytes payload, Net_Endpoint to){
	switch(to.address.family){
		case Net_IPv4: {
			struct sockaddr_in os_endpoint = _unwrap_endpoint_ip4(to);
			printf("Sending to IP4: %08x : %d (%d)\n", os_endpoint.sin_addr.s_addr, os_endpoint.sin_port, htons(9000));
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
			// printf("Sending to IP4: %08x : %d (%d)\n", os_endpoint.sin6_addr, os_endpoint.sin_port, htons(9000));
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

isize net_receive_udp(Net_UDP_Socket sock, Bytes buf, Net_Endpoint* remote){
	alignas(16) byte addr_data[64] = {0}; // Enough to store IPv6
	uint addr_len = 64;
	isize n = recvfrom(sock._handle, buf.data, buf.len, 0, (struct sockaddr*)&addr_data, &addr_len);

	if(remote){
		if(addr_len == sizeof(struct sockaddr_in)){
			struct sockaddr_in os_addr = *(struct sockaddr_in *)(&addr_data);
			if(!arch_is_big_endian()){
				swap_bytes(&os_addr.sin_addr.s_addr);
				swap_bytes(&os_addr.sin_port);
			}

			remote->address.family = Net_IPv4;
			remote->port = os_addr.sin_port;
			mem_copy(&remote->address.data.ip4, &os_addr.sin_addr, 4);
		}
		else if(addr_len == sizeof(struct sockaddr_in6)){
			struct sockaddr_in6 os_addr = *(struct sockaddr_in6 *)(&addr_data);
			if(!arch_is_big_endian()){
				swap_bytes(&os_addr.sin6_addr);
				swap_bytes(&os_addr.sin6_port);
			}

			remote->address.family = Net_IPv6;
			remote->port = os_addr.sin6_port;
			mem_copy(&remote->address.data.ip6, &os_addr.sin6_addr, 16);
		}
	}

	// printf("ADDR LEN: %u\n", addr_len);
	// printf("Ip6 %zu", );
	// printf("Ip4 %zu", sizeof(struct sockaddr_in));
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

    return (Net_Socket){
        .proto = proto,
        ._handle = sock_fd,
    };
}

#else
#error "Unimplemented platform"
#endif


#endif
