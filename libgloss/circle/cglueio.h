#ifndef _CIRCNEWLIB_CGLUEIO_H
#define _CIRCNEWLIB_CGLUEIO_H

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>

namespace _CircleStdlib
{
    class CGlueIO
    {
    public:
        CGlueIO() : mRefCount(1)
        {
        }

        virtual
        ~CGlueIO ()
        {
        }

        virtual int
        Read (void *pBuffer, int nCount) = 0;

        virtual int
        Write (const void *pBuffer, int nCount) = 0;

        virtual int
        LSeek(int ptr, int dir) = 0;

        virtual int
        Close (void) = 0;

        virtual int
        FTruncate (off_t)
        {
            errno = EINVAL;
            return -1;
        }

        virtual int
        FSync (void)
        {
            errno = EINVAL;
            return -1;
        }

        virtual int
        FStat (struct stat *buf) = 0;

        virtual int
        IsATty (void) = 0;

        virtual int
        Bind (const struct sockaddr *,
                socklen_t)
        {
            errno = ENOTSOCK;
            return -1;           
        }

        virtual int
        Listen (int backlog)
        {
            errno = ENOTSOCK;
            return -1;
        }

        void IncrementRefCount (void)
        {
            mRefCount += 1;
        }

        void DecrementRefCount (void)
        {
            assert (mRefCount > 0);
            mRefCount -= 1;
        }

        unsigned int GetRefCount (void) const
        {
            return mRefCount;
        }

    private:
        unsigned int mRefCount;
    };
}

#endif

