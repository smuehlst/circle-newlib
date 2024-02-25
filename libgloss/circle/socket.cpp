#include <circle/net/netsubsystem.h>
#include <circle/sched/scheduler.h>
#include <circle/net/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include "cglueio.h"
#include "filetable.h"
#include "circlenetmap.h"

#include <circle/logger.h>

namespace _CircleStdlib
{

    static CNetSubSystem *pCNet = nullptr;

    /**
     * Posix sockets
     */
    struct CGlueIoSocket : public CGlueIO
    {
        enum socket_state
        {
            socket_state_new,
            socket_state_bound,
            socket_state_listening,
            socket_state_connected
        };

        CGlueIoSocket(int nProtocol)
            : mSocket(new CSocket(pCNet, nProtocol)),
              mState(socket_state_new)
        {
        }

        CGlueIoSocket(CSocket *acceptedSocket)
            : mSocket(acceptedSocket),
              mState(socket_state_connected)
        {
            assert(acceptedSocket);
        }

        ~CGlueIoSocket()
        {
            delete mSocket;
        }

        int
        LSeek(int ptr, int dir)
        {
            return -1;
        }

        int
        Close(void)
        {
            CScheduler::Get()->Yield();

            // Nothing to do here. The Circle socket object will be
            // implicitly closed and destroyed when the CGlueIO object
            // is destroyed.
            return 0;
        }

        int
        FStat(struct stat *)
        {
            errno = EBADF;
            return -1;
        }

        int
        IsATty(void)
        {
            errno = ENOTTY;
            return -1;
        }

        int
        Bind(const struct sockaddr *sa,
             socklen_t len)
        {
            if (sa->sa_family != AF_INET)
            {
                errno = EOPNOTSUPP;
                return -1;
            }

            if (len != sizeof(struct sockaddr_in) || mState != socket_state_new)
            {
                errno = EINVAL;
                return -1;
            }

            const struct sockaddr_in *const sa_in = reinterpret_cast<const struct sockaddr_in *>(sa);

            // Circle has no means of binding to a specific interface
            if (sa_in->sin_addr.s_addr != htonl(INADDR_ANY))
            {
                errno = EADDRNOTAVAIL;
                return -1;
            }

            /*
             * Circle expects the port in little-endian representation, e.g. in host byte order.
             * The socket interface requires the port in network byte order. Therefore we
             * have to convert to host byte order here.
             */
            int const bind_result = mSocket->Bind(ntohs(sa_in->sin_port));

            if (bind_result < 0)
            {
                // We don't know the specific reason for the failure. EACCESS seems like a good generic errno.
                errno = EACCES;
                return -1;
            }

            mState = socket_state_bound;

            CScheduler::Get()->Yield();

            return 0;
        }

        virtual int
        Listen(int backlog)
        {
            if (mState == socket_state_connected)
            {
                errno = EINVAL;
                return -1;
            }

            // The listen() function is documented with the following behaviors:
            //
            // "The implementation may have an upper limit on the length of the
            // listen queue-either global or per accepting socket. If backlog
            // exceeds this limit, the length of the listen queue is set to the limit.
            //
            // If listen() is called with a backlog argument value that is less
            // than 0, the function behaves as if it had been called with a backlog
            // argument value of 0.
            //
            // A backlog argument of 0 may allow the socket to accept connections,
            // in which case the length of the listen queue may be set to an
            // implementation-defined minimum value."
            //
            // Circle's Listen() fails with a backlog of 0, so we use 1 as the
            // implementation-defined minimum value.
            unsigned int const ubacklog =
                backlog < 1
                    ? 1
                    : (backlog > SOMAXCONN
                           ? SOMAXCONN
                           : backlog);

            int const listen_result = mSocket->Listen(ubacklog);
            if (listen_result < 0)
            {
                // We don't know the exact reason.
                errno = ENOBUFS;
            }
            else
            {
                mState = socket_state_listening;
            }

            CScheduler::Get()->Yield();

            return listen_result;
        }

        int Accept(struct sockaddr *address, socklen_t *address_len)
        {
            if (mState != socket_state_listening)
            {
                errno = EINVAL;
                return -1;
            }

            CIPAddress ForeignIP;
            u16 nForeignPort;
            CSocket *const pConnection = mSocket->Accept(&ForeignIP, &nForeignPort);

            if (!pConnection)
            {
                errno = EINVAL;
                return -1;
            }

            _CircleStdlib::CircleFile *circleFile = nullptr;
            int const slot = _CircleStdlib::FileTable::FindFreeFileSlot(circleFile);

            if (slot != -1)
            {
                assert(circleFile != nullptr);

                auto const new_socket = new _CircleStdlib::CGlueIoSocket(pConnection);
                circleFile->AssignGlueIO(*new_socket);

                if (address && address_len && *address_len > 0)
                {
                    struct sockaddr_in sockaddr;
                    socklen_t out_len = sizeof(struct sockaddr_in);
                    socklen_t const in_len = *address_len;

                    *address_len = out_len;
                    if (out_len > in_len)
                    {
                        out_len = in_len;
                    }

                    sockaddr.sin_family = AF_INET;
                    sockaddr.sin_addr.s_addr = (u32)ForeignIP;
                    sockaddr.sin_port = nForeignPort;

                    memcpy(address, &sockaddr, out_len);
                }
            }
            else
            {
                errno = ENFILE;
            }

            CScheduler::Get()->Yield();

            return slot;
        }

        virtual int
        Connect(const struct sockaddr *address, socklen_t address_len)
        {
            if (address->sa_family != AF_INET || mState == socket_state_listening)
            {
                errno = EOPNOTSUPP;
                return -1;
            }

            if (address_len != sizeof(struct sockaddr_in))
            {
                errno = EINVAL;
                return -1;
            }

            if (mState == socket_state_connected)
            {
                errno = EISCONN;
                return -1;
            }

            const struct sockaddr_in * const in_addr = reinterpret_cast<const struct sockaddr_in *>(address);

            CIPAddress circle_address = ntohl(in_addr->sin_addr.s_addr);
            u16 const circle_port = ntohs(in_addr->sin_port);

            CLogger::Get ()->Write ("socket", LogNotice, "addr ext %x addr circle %x\n", in_addr->sin_addr.s_addr, (u32)circle_address);

            int const result = mSocket->Connect(circle_address, circle_port);
            if (result == -1)
            {
                // We don't know better.
                errno = EADDRNOTAVAIL;
            }
            else
            {
                mState = socket_state_connected;
            }

            CScheduler::Get()->Yield();

            return result;
        }

        int
        Read(void *pBuffer, int nCount)
        {
            assert(mSocket);

            if (mState != socket_state_connected)
            {
                errno = ENOTCONN;
                return -1;
            }

            int const result = mSocket->Receive(pBuffer, nCount, 0);

            if (result == -1)
            {
                errno = EPIPE;
            }

            CScheduler::Get()->Yield();

            return result;
        }

        int
        Write(const void *pBuffer, int nCount)
        {
            assert(mSocket);

            if (mState != socket_state_connected)
            {
                errno = ECONNRESET;
                return -1;
            }

            int const result = mSocket->Send(pBuffer, nCount, 0);

            if (result == -1)
            {
                errno = EPIPE;
            }

            CScheduler::Get()->Yield();

            return result;
        }

        CSocket *mSocket;
        socket_state mState;
    };
}

void CGlueNetworkInit(CNetSubSystem &rNetwork)
{
    assert(!_CircleStdlib::pCNet);
    _CircleStdlib::pCNet = &rNetwork;
}

extern "C" int accept(int socket, struct sockaddr *address,
                      socklen_t *address_len)
{
    _CircleStdlib::FileTable::FileTableLock fileTabLock;

    _CircleStdlib::CircleFile *const socket_file = _CircleStdlib::FileTable::GetFile(socket);

    if (!socket_file || !socket_file->IsOpen())
    {
        errno = EBADF;
        return -1;
    }

    _CircleStdlib::CGlueIO *const glueIO = socket_file->GetGlueIO();
    assert(glueIO);

    return glueIO->Accept(address, address_len);
}

extern "C" int bind(int socket, const struct sockaddr *address,
                    socklen_t address_len)
{
    _CircleStdlib::FileTable::FileTableLock fileTabLock;

    _CircleStdlib::CircleFile *const socket_file = _CircleStdlib::FileTable::GetFile(socket);

    if (!socket_file || !socket_file->IsOpen())
    {
        errno = EBADF;
        return -1;
    }

    _CircleStdlib::CGlueIO *const glueIO = socket_file->GetGlueIO();
    assert(glueIO);

    return glueIO->Bind(address, address_len);
}

extern "C" int connect(int socket, const struct sockaddr *address,
                       socklen_t address_len)
{
    _CircleStdlib::FileTable::FileTableLock fileTabLock;

    _CircleStdlib::CircleFile *const socket_file = _CircleStdlib::FileTable::GetFile(socket);

    if (!socket_file || !socket_file->IsOpen())
    {
        errno = EBADF;
        return -1;
    }

    _CircleStdlib::CGlueIO *const glueIO = socket_file->GetGlueIO();
    assert(glueIO);

    return glueIO->Connect(address, address_len);
}

extern "C" int getpeername(int socket, struct sockaddr *address,
                           socklen_t *address_len)
{
    errno = ENOSYS;
    return -1;
}

extern "C" int getsockname(int socket, struct sockaddr *address,
                           socklen_t *address_len)
{
    errno = ENOSYS;
    return -1;
}

extern "C" int getsockopt(int socket, int level, int option_name,
                          void *option_value, socklen_t *option_len)
{
    errno = ENOSYS;
    return -1;
}

extern "C" int listen(int socket, int backlog)
{
    _CircleStdlib::FileTable::FileTableLock fileTabLock;

    _CircleStdlib::CircleFile *const socket_file = _CircleStdlib::FileTable::GetFile(socket);

    if (!socket_file || !socket_file->IsOpen())
    {
        errno = EBADF;
        return -1;
    }

    _CircleStdlib::CGlueIO *const glueIO = socket_file->GetGlueIO();
    assert(glueIO);

    return glueIO->Listen(backlog);
}

extern "C" ssize_t recv(int socket, void *buffer, size_t length, int flags)
{
    errno = ENOSYS;
    return -1;
}

extern "C" ssize_t recvfrom(int socket, void *buffer, size_t length,
                            int flags, struct sockaddr *address, socklen_t *address_len)
{
    errno = ENOSYS;
    return -1;
}

extern "C" ssize_t recvmsg(int socket, struct msghdr *message, int flags)
{
    errno = ENOSYS;
    return -1;
}

extern "C" ssize_t send(int socket, const void *message, size_t length, int flags)
{
    errno = ENOSYS;
    return -1;
}

extern "C" ssize_t sendmsg(int socket, const struct msghdr *message, int flags)
{
    errno = ENOSYS;
    return -1;
}

extern "C" ssize_t sendto(int socket, const void *message, size_t length, int flags,
                          const struct sockaddr *dest_addr, socklen_t dest_len)
{
    errno = ENOSYS;
    return -1;
}

extern "C" int setsockopt(int socket, int level, int option_name,
                          const void *option_value, socklen_t option_len)
{
    errno = ENOSYS;
    return -1;
}

extern "C" int shutdown(int socket, int how)
{
    errno = ENOSYS;
    return -1;
}

extern "C" int socket(int domain, int type, int protocol)
{
    if (domain != AF_INET)
    {
        errno = EAFNOSUPPORT;
        return -1;
    }

    int circle_socket_protocol = 0;
    switch (type)
    {
    case SOCK_STREAM:
        switch (protocol)
        {
        case IPPROTO_TCP:
        case 0:
            circle_socket_protocol = _CircleStdlib::CircleNetMap::C_IPPROTO_TCP;
            break;

        default:
            errno = EPROTONOSUPPORT;
            return -1;
        }
        break;

    case SOCK_DGRAM:
        switch (protocol)
        {
        case IPPROTO_UDP:
        case 0:
            circle_socket_protocol = _CircleStdlib::CircleNetMap::C_IPPROTO_UDP;
            break;

        default:
            errno = EPROTONOSUPPORT;
            return -1;
        }
        break;

    default:
        errno = EPROTOTYPE;
        return -1;
    }

    _CircleStdlib::FileTable::FileTableLock fileTabLock;

    _CircleStdlib::CircleFile *circleFile = nullptr;
    int const slot = _CircleStdlib::FileTable::FindFreeFileSlot(circleFile);

    if (slot != -1)
    {
        assert(circleFile != nullptr);

        auto const new_socket = new _CircleStdlib::CGlueIoSocket(circle_socket_protocol);
        circleFile->AssignGlueIO(*new_socket);
    }
    else
    {
        errno = ENFILE;
    }

    return slot;
}

extern "C" int socketpair(int domain, int type, int protocol,
                          int socket_vector[2])
{
    errno = ENOSYS;
    return -1;
}

extern "C" uint32_t htonl(uint32_t hostlong)
{
    return
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        __builtin_bswap32(hostlong);
#else
        hostlong
#endif
    ;
}

extern "C" uint16_t htons(uint16_t hostshort)
{
    return
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        __builtin_bswap16(hostshort)
#else
        hostshort
#endif
            ;
}

extern "C" uint32_t ntohl(uint32_t netlong)
{
    return
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        __builtin_bswap32(netlong);
#else
        netlong
#endif
    ;
}

extern "C" uint16_t ntohs(uint16_t netshort)
{
    return
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        __builtin_bswap16(netshort)
#else
        netshort
#endif
            ;
}
