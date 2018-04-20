/* Directory entry as returned by readdir */

#ifndef _SYS_DIRENT_H
#define _SYS_DIRENT_H

#include <sys/types.h>

#include <circle/fs/fsdef.h>

typedef struct _CIRCLE_DIR DIR;

struct dirent {
        ino_t  d_ino;
        char   d_name[FS_TITLE_LEN + 1];
};

#if 0
{
    int dd_fd;		/* directory file */
    int dd_loc;		/* position in buffer */
    int dd_seek;
    char *dd_buf;	/* buffer */
    int dd_len;		/* buffer length */
    int dd_size;	/* amount of data in buffer */
    _LOCK_RECURSIVE_T dd_lock;
} DIR;
#endif

/* --- redundant --- */

DIR *opendir(const char *);
struct dirent *readdir(DIR *);
int readdir_r(DIR *__restrict, struct dirent *__restrict,
              struct dirent **__restrict);
void rewinddir(DIR *);
int closedir(DIR *);

/* internal prototype */
#if 0
void _seekdir(DIR *dir, long offset);
DIR *_opendir(const char *);
#endif

#endif
