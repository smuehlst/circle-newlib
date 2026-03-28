#include <net/if.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

static const char s_ifname[] = "eth0";

extern "C"
char *if_indextoname (unsigned ifindex, char *ifname)
{
	if (ifindex != 1)
	{
		errno = ENXIO;

		return nullptr;
	}

	assert (ifname);
	return strncpy (ifname, s_ifname, IF_NAMESIZE);
}

extern "C"
unsigned if_nametoindex (const char *ifname)
{
	assert (ifname);
	if (strcmp (ifname, s_ifname) != 0)
	{
		return 0;
	}

	return 1;
}
