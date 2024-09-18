#pragma once

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

typedef struct {
    i64 _handle;
} Net_Socket;

#ifdef BASE_C_IMPLEMENTATION

#endif