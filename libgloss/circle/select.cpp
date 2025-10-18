#include <sys/select.h>
#include <sys/time.h>
#include <errno.h>
#include <limits.h>
#include "circle_macros.h"
#include "filetable.h"
#include "cglueio.h"
#include <circle/timer.h>

static_assert(_CircleStdlib::FileTable::MAX_OPEN_FILES <= FD_SETSIZE,
              "MAX_OPEN_FILES exceeds FD_SETSIZE");
static_assert(sizeof(fd_set::_x) * CHAR_BIT >= FD_SETSIZE,
              "fd_set too small for MAX_OPEN_FILES");

extern "C" void FD_CLR(int fd, fd_set *fdset)
{
    if (!fdset || fd < 0 || fd >= FD_SETSIZE)
    {
        return;
    }

    int const mask = ~(1 << fd);
    fdset->_x &= mask;
}

extern "C" int FD_ISSET(int fd, fd_set *fdset)
{
    if (!fdset || fd < 0 || fd >= FD_SETSIZE)
    {
        return 0;
    }

    int const mask = 1 << fd;
    return (fdset->_x & mask) != 0;
}

extern "C" void FD_SET(int fd, fd_set *fdset)
{
    if (!fdset || fd < 0 || fd >= FD_SETSIZE)
    {
        return;
    }

    int const mask = 1 << fd;
    fdset->_x |= mask;
}

extern "C" void FD_ZERO(fd_set *fdset)
{
    fdset->_x = 0;
}

extern "C" int pselect(int nfds, fd_set *readfds,
                       fd_set *writefds, fd_set *errorfds,
                       const struct timespec *timeout,
                       const sigset_t *sigmask)
{
    errno = ENOSYS;
    return -1;
}

extern "C" int select(int nfds, fd_set *readfds,
                      fd_set *writefds, fd_set *errorfds,
                      struct timeval *timeout)
{
    using _CircleStdlib::CGlueIO;
    using _CircleStdlib::CircleFile;

    // Validate nfds.
    if (nfds < 0 || nfds > FD_SETSIZE)
    {
        errno = EINVAL;
        return -1;
    }

    auto const prepare_copy = [](fd_set *param) -> fd_set
    {
        if (param)
        {
            return *param;
        }
        fd_set copy;
        FD_ZERO(&copy);
        return copy;
    };

    fd_set read_copy = prepare_copy(readfds);
    fd_set write_copy = prepare_copy(writefds);
    fd_set err_copy = prepare_copy(errorfds);

    fd_set read_out;
    fd_set write_out;
    fd_set err_out;
    FD_ZERO(&read_out);
    FD_ZERO(&write_out);
    FD_ZERO(&err_out);

    // Compute timeout deadline (in clock ticks)
    u64 const start = CTimer::GetClockTicks64();
    u64 const wait_ticks = (timeout ? (u64)timeout->tv_sec * CLOCKHZ + (u64)timeout->tv_usec * (CLOCKHZ / 1000000) : (u64)-1);

    int bit_set_count = 0;
    while (bit_set_count == 0)
    {
        // Check for timeout
        if (timeout && CTimer::GetClockTicks64() - start >= wait_ticks)
        {
            // Timeout reached, return with no bits set
            break;
        }

        for (int fd = 0; fd < nfds; fd += 1)
        {
            CircleFile *const file = _CircleStdlib::FileTable::GetFile(fd);
            if (!file || !file->IsOpen())
            {
                errno = EBADF;
                return -1;
            }

            CGlueIO *const glueIO = file->GetGlueIO();
            assert(glueIO);

            CGlueIO::TStatus const status = glueIO->GetSelectStatus();

            if (FD_ISSET(fd, &read_copy) && status.bRxReady)
            {
                FD_SET(fd, &read_out);
                bit_set_count += 1;
            }
            if (FD_ISSET(fd, &write_copy) && status.bTxReady)
            {
                FD_SET(fd, &write_out);
                bit_set_count += 1;
            }
            if (FD_ISSET(fd, &err_copy) && status.bException)
            {
                FD_SET(fd, &err_out);
                bit_set_count += 1;
            }
        }
    }

    if (readfds)
    {
        *readfds = read_out;
    }
    if (writefds)
    {
        *writefds = write_out;
    }
    if (errorfds)
    {
        *errorfds = err_out;
    }

    return bit_set_count;
}
