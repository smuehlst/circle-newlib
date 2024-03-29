#include "config.h"
#include <_ansi.h>
#include <_syslist.h>
#include <errno.h>
#include <stdio.h>

#include "wrap_fatfs.h"

extern "C"
int
_rename (const char *oldname, const char *newname)
{
    FRESULT const fresult = f_rename (oldname, newname);

    int result;

    switch (fresult)
    {
        case FR_OK:
            result = 0;
            break;

        default:
            result = -1;
            switch (fresult)
            {
                case FR_NO_PATH:
                case FR_NO_FILE:
                case FR_INVALID_NAME:
                case FR_INVALID_DRIVE:
                    errno = ENOENT;
                    break;

                case FR_EXIST:
                case FR_WRITE_PROTECTED:
                    errno = EPERM;
                    break;

                case FR_DISK_ERR:
                case FR_INT_ERR:
                case FR_NOT_READY:
                case FR_NOT_ENABLED:
                case FR_NO_FILESYSTEM:
                case FR_TIMEOUT:
                case FR_LOCKED:
                case FR_NOT_ENOUGH_CORE:
                default:
                    errno = EACCES;
                    break;
            }
    }

    return result;
}
