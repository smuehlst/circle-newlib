#include "filetable.h"

namespace _CircleStdlib
{
    CircleFile FileTable::fileTab[];
    CSpinLock FileTable::fileTabLock { TASK_LEVEL };

    _CIRCLE_DIR FileTable::dirTab[];
    CSpinLock FileTable::dirTabLock { TASK_LEVEL };

    int
    FileTable::FindFreeFileSlot(CircleFile *&pFile)
    {
        int slotNr = -1;

        for (auto &slot : fileTab)
        {
            if (!slot.IsOpen())
            {
                pFile = &slot;
                slotNr = &slot - fileTab;
                break;
            }
        }

        return slotNr;
    }

    CircleFile *FileTable::GetFile(int slot)
    {
        return slot >= 0 && slot < MAX_OPEN_FILES ? fileTab + slot : nullptr;
    }

    int
    FileTable::FindFreeDirSlot(_CIRCLE_DIR *&pDir)
    {
        int slotNr = -1;

        for (auto &slot : dirTab)
        {
            if (!slot.mOpen)
            {
                pDir = &slot;
                slotNr = &slot - dirTab;
                break;
            }
        }

        return slotNr;
    }

    _CIRCLE_DIR *FileTable::GetDir(int slot)
    {
        return slot >= 0 && slot < MAX_OPEN_DIRS ? dirTab + slot : nullptr;
    }
}