#include "config.h"
#include <_ansi.h>
#include <_syslist.h>
#include <errno.h>
#undef errno
extern int errno;
#include "warning.h"

/*
 * Stub version of open.
 */

extern "C"
int
_DEFUN (_open, (file, flags, mode),
        char *file  _AND
        int   flags _AND
        int   mode)
{
  errno = ENOSYS;
  return -1;
}

stub_warning(_open)

/*
 * Stub version of close.
 */

extern "C"
int
_DEFUN (_close, (fildes),
        int fildes)
{
  errno = ENOSYS;
  return -1;
}

stub_warning (_close)

/*
 * Stub version of read.
 */

extern "C"
int
_DEFUN (_read, (file, ptr, len),
        int   file  _AND
        char *ptr   _AND
        int   len)
{
  errno = ENOSYS;
  return -1;
}

stub_warning(_read)


/*
 * Stub version of write.
 */

extern "C"
int
_DEFUN (_write, (file, ptr, len),
        int   file  _AND
        char *ptr   _AND
        int   len)
{
  errno = ENOSYS;
  return -1;
}

stub_warning(_write)

