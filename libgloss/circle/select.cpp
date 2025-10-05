#include <sys/select.h>
#include <errno.h>

extern "C"
void FD_CLR(int fd, fd_set *fdset)
{
}

extern "C"
int FD_ISSET(int fd, fd_set *fdset)
{
    return 0;
}

extern "C"
void FD_SET(int fd, fd_set *fdset)
{
}

extern "C"
void FD_ZERO(fd_set *fdset)
{
}

extern "C"
int pselect(int nfds, fd_set *readfds,
       fd_set *writefds, fd_set *errorfds,
       const struct timespec *timeout,
       const sigset_t *sigmask)
{
    errno = ENOSYS;
    return -1;
}

extern "C"
int select(int nfds, fd_set *readfds,
       fd_set *writefds, fd_set *errorfds,
       struct timeval *timeout)
{
    errno = ENOSYS;
    return -1;
}
