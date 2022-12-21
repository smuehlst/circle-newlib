#ifndef _CIRCNEWLIB_CGLUEIO_H
#define _CIRCNEWLIB_CGLUEIO_H

namespace _CircleStdlib
{

    class CGlueIO
    {
    public:
        virtual ~CGlueIO()
        {
        }

        virtual int
        Read(void *pBuffer, int nCount) = 0;

        virtual int
        Write(const void *pBuffer, int nCount) = 0;

        virtual int
        LSeek(int ptr, int dir) = 0;

        virtual int
        Close(void) = 0;
    };

}

#endif