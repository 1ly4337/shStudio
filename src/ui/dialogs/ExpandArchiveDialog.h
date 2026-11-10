#pragma once
#include "ArchiveDialog.h"
#include "String.hpp"

class ArchiveExtractor;
class FolderRoot;

class ExpandArchiveDialog : public ArchiveDialog
{
public:
    ExpandArchiveDialog(ArchiveExtractor* extractor, FolderRoot* root, const wchar_t* workspace);
    virtual ~ExpandArchiveDialog();

    virtual LPCWSTR getTitle() override;
    virtual void* getWorkerProc() override;
    virtual __int64 getWorkSize() override;

    static DWORD WINAPI threadProc(LPVOID param);

    ArchiveExtractor* m_extractor;
    String      m_workspace;
};
static_assert(sizeof(ExpandArchiveDialog) == 0xFC, "ExpandArchiveDialog size");
