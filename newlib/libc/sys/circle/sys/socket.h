/**
 * socket implementation based on Circle
 */

#ifndef _SYS_SOCKET_H
#define _SYS_SOCKET_H

typedef int socklen_t;
typedef unsigned int sa_family_t;

struct sockaddr  {
    sa_family_t  sa_family; /* Address family. */
    char         sa_data[]; /* Socket address (variable-length data). */
};

/* TODO alignment issues? */
struct sockaddr_storage {
    sa_family_t   ss_family;
};

struct cmsghdr {
    socklen_t  cmsg_len;    /* Data byte count, including the cmsghdr. */
    int        cmsg_level;  /* Originating protocol. */
    int        cmsg_type;   /* Protocol-specific type. */   
};

struct linger {
    int  l_onoff;   /* Indicates whether linger option is enabled. */
    int  l_linger;  /* Linger time, in seconds. */
};

struct msghdr {
    void          *msg_name;        /* Optional address. */
    socklen_t      msg_namelen;     /* Size of address. */
    struct iovec  *msg_iov;         /* Scatter/gather array. */
    int            msg_iovlen;      /* Members in msg_iov. */
    void          *msg_control;     /* Ancillary data. */
    socklen_t      msg_controllen;  /* Ancillary data buffer len. */
    int            msg_flags;       /* Flags on received message. */
};

int     accept(int, struct sockaddr *, socklen_t *);
int     bind(int, const struct sockaddr *, socklen_t);
int     connect(int, const struct sockaddr *, socklen_t);
int     getpeername(int, struct sockaddr *, socklen_t *);
int     getsockname(int, struct sockaddr *, socklen_t *);
int     getsockopt(int, int, int, void *, socklen_t *);
int     listen(int, int);
ssize_t recv(int, void *, size_t, int);
ssize_t recvfrom(int, void *, size_t, int,
        struct sockaddr *, socklen_t *);
ssize_t recvmsg(int, struct msghdr *, int);
ssize_t send(int, const void *, size_t, int);
ssize_t sendmsg(int, const struct msghdr *, int);
ssize_t sendto(int, const void *, size_t, int, const struct sockaddr *,
        socklen_t);
int     setsockopt(int, int, int, const void *, socklen_t);
int     shutdown(int, int);
int     socket(int, int, int);
int     sockatmark(int);
int     socketpair(int, int, int, int[2]);

void FD_CLR(int fd, fd_set *fdset);
int FD_ISSET(int fd, fd_set *fdset);
void FD_SET(int fd, fd_set *fdset);
void FD_ZERO(fd_set *fdset);

/* */
#define SOCK_DGRAM  1                   /* Datagram socket. */
#define SOCK_RAW    (SOCK_DGRAM + 1)    /* Raw Protocol Interface. */
#define SOCK_SEQPACKET (SOCK_DGRAM + 2) /* Sequenced-packet socket. */
#define SOCK_STREAM (SOCK_DGRAM + 3)    /* Byte-stream socket. */

/* level argument of setsockopt() and getsockopt() */
#define SOL_SOCKET  1

/* Maximum backlog queue length which may be specified by the backlog field of the listen() function */
#define SOMAXCONN   0   /* TODO determine value */

/* option_name argument in getsockopt() or setsockopt() calls: */
#define SO_ACCEPTCONN	1	                /* Socket is accepting connections. */
#define SO_BROADCAST	(SO_ACCEPTCONN + 1)	/* Transmission of broadcast messages is supported. */
#define SO_DEBUG		(SO_ACCEPTCONN + 2)	/* Debugging information is being recorded. */
#define SO_DONTROUTE	(SO_ACCEPTCONN + 3)	/* Bypass normal routing. */
#define SO_ERROR		(SO_ACCEPTCONN + 4)	/* Socket error status. */
#define SO_KEEPALIVE	(SO_ACCEPTCONN + 5)	/* Connections are kept alive with periodic messages. */
#define SO_LINGER		(SO_ACCEPTCONN + 6)	/* Socket lingers on close. */
#define SO_OOBINLINE	(SO_ACCEPTCONN + 7)	/* Out-of-band data is transmitted in line. */
#define SO_RCVBUF		(SO_ACCEPTCONN + 8)	/* Receive buffer size. */
#define SO_RCVLOWAT		(SO_ACCEPTCONN + 9)	/* Receive ``low water mark''. */
#define SO_RCVTIMEO		(SO_ACCEPTCONN + 10)	/* Receive timeout. */
#define SO_REUSEADDR	(SO_ACCEPTCONN + 11)	/* Reuse of local addresses is supported. */
#define SO_SNDBUF		(SO_ACCEPTCONN + 12)	/* Send buffer size. */
#define SO_SNDLOWAT		(SO_ACCEPTCONN + 13)	/* Send ``low water mark''. */
#define SO_SNDTIMEO		(SO_ACCEPTCONN + 14)	/* Send timeout. */
#define SO_TYPE		    (SO_ACCEPTCONN + 15)	/* Socket type. */

/* Values for the msg_flags field in the msghdr structure, or the flags parameter in recvfrom(): */
#define MSG_CTRUNC		1	                /* Control data truncated. */
#define MSG_DONTROUTE	(MSG_CTRUNC + 1)	/* Send without using routing tables. */
#define MSG_EOR		    (MSG_CTRUNC + 2)	/* Terminates a record (if supported by the protocol). */
#define MSG_OOB		    (MSG_CTRUNC + 3)	/* Out-of-band data. */
#define MSG_PEEK		(MSG_CTRUNC + 4)	/* Leave received data in queue. */
#define MSG_TRUNC		(MSG_CTRUNC + 5)	/* Normal data truncated. */
#define MSG_WAITALL		(MSG_CTRUNC + 6)	/* Attempt to fill the read buffer. */

/* */
#define AF_INET     1               /* Internet domain sockets for use with IPv4 addresses. */
#define AF_INET6    (AF_INET + 1)   /* Internet domain sockets for use with IPv6 addresses. */
#define AF_UNIX     (AF_INET + 2)   /* UNIX domain sockets. */
#define AF_UNSPEC   (AF_INET + 3)   /* Unspecified. */

/* Type of shutdown for shutdown() "how" argument: */
#define SHUT_RD     1               /* Disables further receive operations. */
#define SHUT_WR     (SHUT_RD + 1)   /* Disables further send operations. */
#define SHUT_RDWR   (SHUT_RD + 2)   /* Disables further send and receive operations. */

/* TODO alignment definitions, dummies for now */
/* 
 * If the argument is a pointer to a cmsghdr structure, this macro shall return
 * an unsigned character pointer to the data array associated with the cmsghdr
 * structure.
 */
#define CMSG_DATA(cmsg) ((void *) 0)

/*
 * If the first argument is a pointer to a msghdr structure and the second argument
 * is a pointer to a cmsghdr structure in the ancillary data pointed to by the
 * msg_control field of that msghdr structure, this macro shall return a pointer
 * to the next cmsghdr structure, or a null pointer if this structure is the last
 * cmsghdr in the ancillary data.
 */
#define CMSG_NXTHDR(mhdr,cmsg)  ((void *) 0)

/*
 * If the argument is a pointer to a msghdr structure, this macro shall return a
 * pointer to the first cmsghdr structure in the ancillary data associated with
 * this msghdr structure, or a null pointer if there is no ancillary data
 * associated with the msghdr structure.
 */
#define CMSG_FIRSTHDR(mhdr)     ((void *) 0)

#endif