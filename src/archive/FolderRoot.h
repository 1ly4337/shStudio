#pragma once
#include <windows.h>
#include "FolderTree.h"

class StreamNotifier;

#pragma vtordisp(push, 2)

class FolderRoot : public FolderTree
{
public:
    FolderRoot();

    virtual void reset() override;
    virtual FileTree* findFile(const wchar_t* path) override;
    virtual DWORD rescan(StreamNotifier* notifier);
    virtual FolderTree* findFolderPath(const wchar_t* path);
};

#pragma vtordisp(pop)
static_assert(sizeof(FolderRoot) == 0x98, "FolderRoot size");
