#include "ReadArchiveDialog.h"
#include "SahArchive.h"
#include "FolderRoot.h"
#include "ida_compat.h"
#include "_imports_p09.h"
#include "_imports_g3.h"

DWORD WINAPI ReadArchiveDialog::threadProc(LPVOID param)
{
    ReadArchiveDialog* dialog = (ReadArchiveDialog*)param;
    if (!dialog)
        return E_POINTER;
    dialog->m_root->reset();
    int result = dialog->m_archive->readIndex(static_cast<StreamNotifier*>(dialog));
    dialog->m_result = result;
    FIELD(HANDLE, dialog, 0xC4) = 0;
    PostMessageW(dialog->m_hWnd, WM_COMMAND, 2 * (result != 0) + 1, 0);
    return 0;
}

LPCWSTR ReadArchiveDialog::getTitle()
{
    return L"Loading Archive";
}

void* ReadArchiveDialog::getWorkerProc()
{
    return (void*)threadProc;
}

__int64 ReadArchiveDialog::getWorkSize()
{
    return m_archive->m_fileCount;
}
