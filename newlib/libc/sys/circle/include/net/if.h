#ifndef _NET_IF_H_
#define _NET_IF_H_

#ifdef __cplusplus
extern "C" {
#endif

#define IF_NAMESIZE 16

char *if_indextoname(unsigned ifindex, char *ifname);
unsigned if_nametoindex(const char *ifname);

#ifdef __cplusplus
}
#endif

#endif
