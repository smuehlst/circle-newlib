#include <circle/machineinfo.h>
#include <errno.h>
#include <unistd.h>

extern "C" long sysconf(int name)
{
    switch (name)
    {
    case _SC_NPROCESSORS_ONLN:
    case _SC_NPROCESSORS_CONF:
        return CMachineInfo::Get()->GetCoreCount();

    case _SC_PAGESIZE:
        return 4096; // 4KB page size

    default:
        errno = EINVAL;
        return -1;
    }
}
