#include "Modal.h"
#include "_imports_p06.h"

bool Modal::onCommand(UINT code, HWND ctrl, UINT id)
{
    if (code == 0 && id <= 11)
    {
        m_result = mapCommandToResult(id);
        if (m_result == 1)
        {
            if (m_modelessOwner)
                SendMessageW(m_modelessOwner->m_hWnd, WM_USER + 3, 1, 0);
        }
        if (m_result != -1)
            PostMessageW(m_hWnd, WM_CLOSE, 0, 0);
    }
    return true;
}
