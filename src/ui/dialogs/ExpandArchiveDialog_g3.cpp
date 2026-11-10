#include "ExpandArchiveDialog.h"
#include "ArchiveExtractor.h"
#include "FolderRoot.h"
#include "AlertDial.h"
#include "String.hpp"
#include "ida_compat.h"
#include "_imports_p09.h"
#include "_imports_g3.h"

DWORD WINAPI ExpandArchiveDialog::threadProc(LPVOID param)
{
    ExpandArchiveDialog* dialog = (ExpandArchiveDialog*)param;
    if (!dialog)
        return E_POINTER;
    ArchiveExtractor* extractor = dialog->m_extractor;
    int result = extractor->extract(dialog->m_root, dialog->m_workspace.m_buffer,
                                    static_cast<StreamNotifier*>(dialog));
    dialog->m_result = result;
    FIELD(HANDLE, dialog, 0xC4) = 0;
    if (result == 1223)
    {
        PostMessageW(dialog->m_hWnd, WM_COMMAND, 3, 0);
        return 0;
    }
    String message;
    if (result == 0)
    {
        message << "Archive successfully expanded.\n" << (long)extractor->m_extracted << " files extracted.";
        AlertDial::alert(dialog->m_hWnd, message.m_buffer, 0, 0x40);
        PostMessageW(dialog->m_hWnd, WM_COMMAND, 1, 0);
    }
    else
    {
        message << "One or more errors has occurred.\n" << extractor->m_errors;
        AlertDial::alert(dialog->m_hWnd, message.m_buffer, 0, 0x10);
        PostMessageW(dialog->m_hWnd, WM_COMMAND, 3, 0);
    }
    return 0;
}
