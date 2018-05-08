#ifndef _SYS_DIRENT_H
#define _SYS_DIRENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

#include <circle/fs/fsdef.h>

typedef struct _CIRCLE_DIR DIR;

/* Directory entry as returned by readdir */
struct dirent {
        ino_t  d_ino;
        char   d_name[FS_TITLE_LEN + 1];
};

DIR *opendir(const char *);
struct dirent *readdir(DIR *);
int readdir_r(DIR *__restrict, struct dirent *__restrict,
              struct dirent **__restrict);
void rewinddir(DIR *);
int closedir(DIR *);

#ifdef __cplusplus
}
#endif

#endif
