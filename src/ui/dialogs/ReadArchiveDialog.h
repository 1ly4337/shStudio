#pragma once
#include <windows.h>
#include "ArchiveDialog.h"

class SahArchive;
class FolderRoot;

class ReadArchiveDialog : public ArchiveDialog
{
public:
    ReadArchiveDialog(FolderRoot* root, SahArchive* archive) : ArchiveDialog(root, false), m_archive(archive) {}
    virtual ~ReadArchiveDialog() {}

    virtual LPCWSTR getTitle() override;
    virtual void* getWorkerProc() override;
    virtual __int64 getWorkSize() override;

    static DWORD WINAPI threadProc(LPVOID param);

    SahArchive* m_archive;
};
static_assert(sizeof(ReadArchiveDialog) == 0xDC, "ReadArchiveDialog size");
