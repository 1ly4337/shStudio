#pragma once
#include "ArchiveDialog.h"

class ArchiveWriter;

class PackArchiveDialog : public ArchiveDialog {
public:
    PackArchiveDialog(FolderRoot* root, bool packing, ArchiveWriter* target)
        : ArchiveDialog(root, packing), m_target(target) {}

    LPCWSTR getTitle() override;
    void* getWorkerProc() override;
    __int64 getWorkSize() override;
    static DWORD WINAPI threadProc(LPVOID param);

    ArchiveWriter* m_target;
};
static_assert(sizeof(PackArchiveDialog) == 0xDC, "PackArchiveDialog size");
