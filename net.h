#pragma once

#include "io.h"
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

typedef enum {
    Transport_TCP,
    Transport_UDP,
} Net_Transport_Protocol;

typedef struct {
    Net_Address ip;
    u16 port;
} Net_Endpoint;

// Handle to an OS socket
typedef struct {
    i64 _handle;
} Net_Socket;

// Represents any socket, includes protocol information
typedef struct {
    Net_Transport_Protocol proto;
    i64 _handle;
} Net_Any_Socket; 

// TCP Socket
typedef struct { i64 _handle; } Net_TCP_Socket;

// UDP Socket
typedef struct { i64 _handle; } Net_UDP_Socket;

// Send payload to TCP socket. Returns number of bytes sent. If the number is
// negative, that means an error has occoured and it can be cast to an IO_Error.
isize net_send_tcp(Net_TCP_Socket sock, Bytes payload);

// Send payload to UDP socket. Returns number of bytes sent. If the number is
// negative, that means an error has occoured and it can be cast to an IO_Error.
isize net_send_udp(Net_UDP_Socket sock, Bytes payload);

// Read from TCP socket into buffer. Return number of bytes read, if the number
// is negative an error happened.
isize net_receive_tcp(Net_TCP_Socket sock, Bytes buf);

// Read from UDP socket into buffer. Return number of bytes read, if the number
// is negative an error happened.
isize net_receive_udp(Net_UDP_Socket sock, Bytes buf);

#ifdef BASE_C_IMPLEMENTATION

#endif