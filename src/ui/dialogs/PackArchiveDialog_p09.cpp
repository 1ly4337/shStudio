#include "PackArchiveDialog.h"
#include "ArchiveWriter.h"
#include "FolderRoot.h"
#include "ida_compat.h"
#include "_imports_p09.h"
#include "_imports_g3.h"

DWORD WINAPI PackArchiveDialog::threadProc(LPVOID param)
{
    PackArchiveDialog* dialog = (PackArchiveDialog*)param;
    if (!dialog)
        return E_POINTER;
    ArchiveWriter* writer = (ArchiveWriter*)dialog->m_target;
    int result = writer->write(dialog->m_root, static_cast<StreamNotifier*>(dialog));
    dialog->m_result = result;
    FIELD(HANDLE, dialog, 0xC4) = 0;
    PostMessageW(dialog->m_hWnd, WM_COMMAND, 2 * (result != 0) + 1, 0);
    return 0;
}
