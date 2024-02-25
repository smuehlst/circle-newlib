#ifndef _NETINET_IN_H_
#define _NETINET_IN_H_

#include <inttypes.h>
#include <bits/sockettypes.h>

typedef	uint16_t in_port_t;
typedef uint32_t in_addr_t;

struct in_addr {
    in_addr_t s_addr;
};

/* The sin_port and sin_addr members shall be in network byte order (big-endia). */
struct sockaddr_in {
    sa_family_t sin_family; 
    in_port_t sin_port;
    struct in_addr sin_addr;
};

#define IPPROTO_IP      1                   /* Internet protocol. */
#define IPPROTO_IPV6    (IPPROTO_IP + 1)    /* Internet Protocol Version 6. */
#define IPPROTO_ICMP    (IPPROTO_IP + 2)    /* Control message protocol. */
#define IPPROTO_RAW     (IPPROTO_IP + 3)    /* Raw IP Packets Protocol. */
#define IPPROTO_TCP     (IPPROTO_IP + 4)    /* Transmission control protocol. */
#define IPPROTO_UDP     (IPPROTO_IP + 5)    /* User datagram protocol. */

#define	INADDR_ANY		((in_addr_t) 0x00000000) /* IPv4 wildcard address. */
#define	INADDR_LOOPBACK	((in_addr_t) 0x7f000001) /* IPv4 local host address. */
#define	INADDR_BROADCAST ((in_addr_t) 0xffffffff) /* IPv4 broadcast address. */

#define INET_ADDRSTRLEN 16                  /* Length of the string form for IP. */

#endif