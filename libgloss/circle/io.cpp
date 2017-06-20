#include "config.h"
#include <_ansi.h>
#include <_syslist.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#undef errno
extern int errno;
#include "warning.h"

#include <circle/fs/fat/fatfs.h>
#include <circle_glue.h>
#include <assert.h>

namespace
{
	struct CircleFile
    {
    	CircleFile() : mCGlueIO(nullptr) {}
    	CGlueIO *mCGlueIO;
    };

    unsigned int const MAX_OPEN_FILES = 20;

    CFATFileSystem *circle_fat_fs = nullptr;

    CircleFile fileTab[MAX_OPEN_FILES];

    int FindFreeSlot(void)
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
}

void CGlueStdioInit(CFATFileSystem& rFATFileSystem)
{
    // Must only be called once
    assert(!circle_fat_fs);

    circle_fat_fs = &rFATFileSystem;
}

extern "C"
int
_DEFUN (_open, (file, flags, mode),
		char *file _AND
		int flags _AND
		int mode)
{
	int slot = -1;

	// Only supported modes are read and write. The mask is
	// determined from the newlib header.
	int  const masked_flags = flags & 7;
	if (masked_flags != O_RDONLY && masked_flags != O_WRONLY)
	{
		errno = ENOSYS;
	}
	else
	{
		slot = FindFreeSlot();

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
_DEFUN (_close, (fildes),
        int fildes)
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
_DEFUN (_read, (fildes, ptr, len),
        int   fildes  _AND
        char *ptr   _AND
        int   len)
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

	if (read_result == static_cast<unsigned>(-1))
	{
		errno = EIO;
		return -1;
	}

	return static_cast<int>(read_result);
}

extern "C"
int
_DEFUN (_write, (fildes, ptr, len),
        int   fildes  _AND
        char *ptr   _AND
        int   len)
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

	if (write_result == static_cast<unsigned>(-1))
	{
		errno = EIO;
		return -1;
	}

	return static_cast<int>(write_result);
}
