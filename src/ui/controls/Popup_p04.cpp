#include <windows.h>
#include <cstring>
#include "Popup.h"
#include "ida_compat.h"
#include "_imports_p04.h"

bool Popup::postCommand(UINT id)
{
    return onCommand(0, NULL, id);
}

void Popup::onRegisterClass(WNDCLASSEXW* wcx)
{
    int iconId = FIELD(int, this, 0x44);
    if (iconId > 0)
    {
        WORD resId = (WORD)iconId;
        wcx->hIcon = (HICON)LoadImageW(hModule, MAKEINTRESOURCEW(resId), IMAGE_ICON,
                                       GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), LR_SHARED);
        wcx->hIconSm = (HICON)LoadImageW(hModule, MAKEINTRESOURCEW(resId), IMAGE_ICON,
                                         GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_SHARED);
    }
}

DWORD Popup::getStyle()
{
    return WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | (WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX);
}

DWORD Popup::getExStyle()
{
    return WS_EX_WINDOWEDGE;
}

void Popup::onResized()
{
    HWND child = GetDlgItem(m_hWnd, 101);
    if (child)
        SendMessageW(child, WM_EXITSIZEMOVE, 0, 0);
}

void Popup::onEnterSizeMove()
{
    HWND child = GetDlgItem(m_hWnd, 101);
    if (child)
        SendMessageW(child, WM_ENTERSIZEMOVE, 0, 0);
}

void Popup::onFocus(bool focused)
{
    HWND child = GetDlgItem(m_hWnd, 101);
    if (child)
        SendMessageW(child, (focused == 0) + WM_SETFOCUS, 0, 0);
}

bool Popup::appendMenuItem(HMENU menu, UINT id)
{
    LPWSTR text = (LPWSTR)getCommandHelp(id);
    if (!text)
        return false;
    MENUITEMINFOW mi;
    memset(&mi, 0, sizeof(mi));
    mi.cbSize = sizeof(MENUITEMINFOW);
    mi.fMask = MIIM_STATE | MIIM_ID | MIIM_STRING;
    mi.fType = MFT_STRING;
    mi.wID = id;
    mi.fState = 0;
    mi.dwTypeData = text;
    return InsertMenuItemW(menu, (UINT)-1, FALSE, &mi) != 0;
}
