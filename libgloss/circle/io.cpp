#include "config.h"
#include <_ansi.h>
#include <_syslist.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#undef errno
extern int errno;
#include "warning.h"

#include <circle/fs/fat/fatfs.h>
#include <circle/input/console.h>
#include <circle/string.h>
#include "circle_glue.h"
#include <assert.h>

struct _CIRCLE_DIR {
        _CIRCLE_DIR() :
                mFirstRead(0), mOpen(0)
        {
                mEntry.d_ino = 0;
                mEntry.d_name[0] = 0;
        }

        TFindCurrentEntry mCurrentEntry;
        struct dirent mEntry;
        unsigned int mFirstRead : 1;
        unsigned int mOpen : 1;

};

namespace
{
    struct CircleFile
    {
    	CircleFile() : mCGlueIO(nullptr) {}
    	CGlueIO *mCGlueIO;
    };

    constexpr unsigned int MAX_OPEN_FILES = 20;
    constexpr unsigned int MAX_OPEN_DIRS = 20;

    CFATFileSystem *circle_fat_fs = nullptr;

    CircleFile fileTab[MAX_OPEN_FILES];
    _CIRCLE_DIR dirTab[MAX_OPEN_DIRS];

    int FindFreeFileSlot(void)
    {
    	int slotNr = -1;

    	for (auto const& slot: fileTab)
    	{
    		if (slot.mCGlueIO == nullptr)
    		{
    			slotNr = &slot - fileTab;
    			break;
    		}
    	}

    	return slotNr;
    }

    int FindFreeDirSlot(void)
    {
    	int slotNr = -1;

    	for (auto const& slot: dirTab)
    	{
    		if (!slot.mOpen)
    		{
    			slotNr = &slot - dirTab;
    			break;
    		}
    	}

    	return slotNr;
    }

    void
    CGlueInitFileSystem (CFATFileSystem& rFATFileSystem)
    {
            // Must only be called once
            assert (!circle_fat_fs);

            circle_fat_fs = &rFATFileSystem;
    }

    void
    CGlueInitConsole (CConsole& rConsole)
    {
            CircleFile &stdin = fileTab[0];
            CircleFile &stdout = fileTab[1];
            CircleFile &stderr = fileTab[2];

            // Must only be called once and not be called after a file has already been opened
            assert (!stdin.mCGlueIO);
            assert (!stdout.mCGlueIO);
            assert (!stderr.mCGlueIO);

            stdin.mCGlueIO = new CGlueConsole (rConsole, CGlueConsole::ConsoleModeRead);
            stdout.mCGlueIO = new CGlueConsole (rConsole, CGlueConsole::ConsoleModeWrite);
            stderr.mCGlueIO = new CGlueConsole (rConsole, CGlueConsole::ConsoleModeWrite);
    }
}

void CGlueStdioInit(CFATFileSystem& rFATFileSystem, CConsole& rConsole)
{
        CGlueInitConsole (rConsole);
        CGlueInitFileSystem (rFATFileSystem);
}

void CGlueStdioInit (CFATFileSystem& rFATFileSystem)
{
        CGlueInitFileSystem (rFATFileSystem);
}

void CGlueStdioInit (CConsole& rConsole)
{
        CGlueInitConsole (rConsole);
}

extern "C"
int
_open(char *file, int flags, int mode)
{
	int slot = -1;

	// Only supported modes are read and write. The mask is
	// determined from the newlib header.
	int const masked_flags = flags & 7;
	if (masked_flags != O_RDONLY && masked_flags != O_WRONLY)
	{
		errno = ENOSYS;
	}
	else
	{
		slot = FindFreeFileSlot();

		if (slot != -1)
		{
			CircleFile& newFile = fileTab[slot];
			unsigned handle;
			if (masked_flags == O_RDONLY)
			{
				handle = circle_fat_fs->FileOpen (file);
			}
			else
			{
				assert(masked_flags ==  O_WRONLY);
				handle = circle_fat_fs->FileCreate (file);
			}
			if (handle != 0)
			{
				newFile.mCGlueIO = new CGlueIoFatFs(*circle_fat_fs, handle);
			}
			else
			{
				slot = -1;
				errno = EACCES;
			}
		}
		else
		{
			errno = ENFILE;
		}
	}

	return slot;
}

extern "C"
int
_close(int fildes)
{
	if (fildes < 0 || static_cast<unsigned int>(fildes) >= MAX_OPEN_FILES)
	{
		errno = EBADF;
		return -1;
	}

	CircleFile& file = fileTab[fildes];
	if (file.mCGlueIO == nullptr)
	{
		errno = EBADF;
		return -1;
	}

	unsigned const circle_close_result = file.mCGlueIO->Close();

	delete file.mCGlueIO;
	file.mCGlueIO = nullptr;

	if (circle_close_result == 0)
	{
		errno = EIO;
		return -1;
	}

	return 0;
}

extern "C"
int
_read(int fildes, char *ptr, int len)
{
	if (fildes < 0 || static_cast<unsigned int>(fildes) >= MAX_OPEN_FILES)
	{
		errno = EBADF;
		return -1;
	}

	CircleFile& file = fileTab[fildes];
	if (file.mCGlueIO == nullptr)
	{
		errno = EBADF;
		return -1;
	}

	unsigned const read_result = file.mCGlueIO->Read(ptr, static_cast<unsigned>(len));

	if (read_result == CGlueIO::GeneralFailure)
	{
		errno = EIO;
		return -1;
	}

	return static_cast<int>(read_result);
}

extern "C"
int
_write(int fildes, char *ptr, int len)
{
	if (fildes < 0 || static_cast<unsigned int>(fildes) >= MAX_OPEN_FILES)
	{
		errno = EBADF;
		return -1;
	}

	CircleFile& file = fileTab[fildes];
	if (file.mCGlueIO == nullptr)
	{
		errno = EBADF;
		return -1;
	}

	unsigned const write_result = file.mCGlueIO->Write(ptr, static_cast<unsigned>(len));

	if (write_result == CGlueIO::GeneralFailure)
	{
		errno = EIO;
		return -1;
	}

	return static_cast<int>(write_result);
}

extern "C"
DIR *
opendir (const char *name)
{
        assert (circle_fat_fs);

        /* For now only the single root directory and the current directory are supported */
        if (strcmp(name, "/") != 0 && strcmp(name, ".") != 0)
        {
                errno = ENOENT;
                return 0;
        }

        int const slotNum = FindFreeDirSlot ();
        if (slotNum == -1)
        {
                errno = ENFILE;
                return 0;
        }

        auto &slot = dirTab[slotNum];

        slot.mOpen = 1;
        slot.mFirstRead = 1;

        return &slot;
}

static struct dirent *
do_readdir (DIR *dir, struct dirent *de)
{
        TDirentry Direntry;
        bool haveEntry;
        if (dir->mFirstRead)
        {
                haveEntry = circle_fat_fs->RootFindFirst (&Direntry, &dir->mCurrentEntry);
                dir->mFirstRead = 0;
        }
        else
        {
                haveEntry = circle_fat_fs->RootFindNext (&Direntry, &dir->mCurrentEntry);
        }

        struct dirent *result;
        if (haveEntry)
        {
                memcpy (de->d_name, Direntry.chTitle, sizeof(de->d_name));
                de->d_ino = 0; // TODO: how to determine an inode number in Circle?
                result = de;
        }
        else
        {
                // end of directory does not change errno
                result = nullptr;
        }

        return result;
}

extern "C" struct dirent *
readdir (DIR *dir)
{
        struct dirent *result;

        if (dir->mOpen)
        {
                result = do_readdir (dir, &dir->mEntry);
        }
        else
        {
                errno = EBADF;
                result = nullptr;
        }

        return result;
}

extern "C" int
readdir_r (DIR *__restrict dir, dirent *__restrict de, dirent **__restrict ode)
{
        int result;

        if (dir->mOpen)
        {
                *ode = do_readdir (dir, de);
                result = 0;
        }
        else
        {
                *ode = nullptr;
                result = EBADF;
        }

        return result;
}

extern "C" void
rewinddir (DIR *dir)
{
        dir->mFirstRead = 1;
}

extern "C" int
closedir (DIR *dir)
{
        if (!dir->mOpen)
        {
                errno = EBADF;
                return -1;
        }

        dir->mOpen = 0;
        return 0;
}
