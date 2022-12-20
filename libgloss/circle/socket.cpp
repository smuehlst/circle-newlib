#include <sys/socket.h>
#include <errno.h>

extern "C"
int accept(int socket, struct sockaddr *address,
                      socklen_t *address_len)
{
    errno = ENOSYS;
    return -1;
}

extern "C"
int bind(int socket, const struct sockaddr *address,
                    socklen_t address_len)
{
    errno = ENOSYS;
    return -1;
}

extern "C"
int connect(int socket, const struct sockaddr *address,
                       socklen_t address_len)
{
    errno = ENOSYS;
    return -1;
}

extern "C"
int getpeername(int socket, struct sockaddr *address,
                           socklen_t *address_len)
{
    errno = ENOSYS;
    return -1;

}

extern "C"
int getsockname(int socket, struct sockaddr *address,
                           socklen_t *address_len)
{
    errno = ENOSYS;
    return -1;
}

extern "C"
int getsockopt(int socket, int level, int option_name,
                          void *option_value, socklen_t *option_len)
{
    errno = ENOSYS;
    return -1;
}

extern "C"
int listen(int socket, int backlog)
{
    errno = ENOSYS;
    return -1;
}

extern "C"
ssize_t recv(int socket, void *buffer, size_t length, int flags)
{
    errno = ENOSYS;
    return -1;
}

extern "C"
ssize_t recvfrom(int socket, void *buffer, size_t length,
                            int flags, struct sockaddr *address, socklen_t *address_len)
{
    errno = ENOSYS;
    return -1;
}

extern "C"
ssize_t recvmsg(int socket, struct msghdr *message, int flags)
{
    errno = ENOSYS;
    return -1;
}

extern "C"
ssize_t send(int socket, const void *message, size_t length, int flags)
{
    errno = ENOSYS;
    return -1;
}

extern "C"
ssize_t sendmsg(int socket, const struct msghdr *message, int flags)
{
    errno = ENOSYS;
    return -1;
}

extern "C"
ssize_t sendto(int socket, const void *message, size_t length, int flags,
                          const struct sockaddr *dest_addr, socklen_t dest_len)
{
    errno = ENOSYS;
    return -1;
}

extern "C"
int setsockopt(int socket, int level, int option_name,
                          const void *option_value, socklen_t option_len)
{
    errno = ENOSYS;
    return -1;
}

extern "C"
int shutdown(int socket, int how)
{
    errno = ENOSYS;
    return -1;
}

extern "C"
int socket(int domain, int type, int protocol)
{
    errno = ENOSYS;
    return -1;
}

extern "C"
int socketpair(int domain, int type, int protocol,
                          int socket_vector[2])
{
    errno = ENOSYS;
    return -1;
}
