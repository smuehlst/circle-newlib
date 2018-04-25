/*
 * Stub version of gettimeofday.
 */

#include "config.h"
#include <_ansi.h>
#include <_syslist.h>
#include <sys/time.h>
#include <sys/times.h>
#include <errno.h>
#include "warning.h"

#include <circle/timer.h>

extern "C"
int
_DEFUN (_gettimeofday, (ptimeval, ptimezone),
        struct timeval *ptimeval _AND
        void *ptimezone)
{
        ptimeval->tv_sec = CTimer::Get ()->GetUniversalTime ();
        ptimeval->tv_usec = 0;

        return 0;
}
