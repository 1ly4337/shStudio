#include "Modal.h"
#include "glView.h"
#include "Application.h"
#include "Window.h"
#include "p10_vcall.h"
#include "_imports_p10.h"

Modal* g_currentModal = 0;

static BOOL __stdcall Modal_moveWindow(HWND hWnd, int x, int y)
{
    return SetWindowPos(hWnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

Modal::Modal()
{
    m_bkColor = Window_isClassicAppearance() ? GetSysColor(COLOR_3DFACE) : 0xF0F0F0;
    m_owner = NULL;
    m_modelessOwner = NULL;
    m_result = 0;
    m_running = false;
    m_buttons = 0;
    m_btnWidth = 80;
    m_btnMarginBottom = 40;
    m_btnMarginRight = 20;
    m_rightAlignButtons = true;
    m_btnX = 0;
    m_btnIgnore = NULL;
    m_btnRetry = NULL;
    m_btnAbort = NULL;
    m_btnNo = NULL;
    m_btnYes = NULL;
    m_btnCancel = NULL;
    m_btnOk = NULL;
}

HWND Modal_getTopLevelWindow(HWND hWnd)
{
    HWND wnd = hWnd;
    if (GetWindowLongW(hWnd, GWL_STYLE) & WS_CHILD)
    {
        do
        {
            HWND parent = GetParent(wnd);
            if (!parent)
                break;
            wnd = parent;
        } while (GetWindowLongW(wnd, GWL_STYLE) & WS_CHILD);
    }
    return wnd;
}

bool Modal::createDialog(HWND parent, int cx, int cy, int type, LPCWSTR title)
{
    DWORD extra;
    if (type == -1 || type == 15)
    {
        m_buttons = -1;
        extra = WS_SYSMENU;
    }
    else
    {
        m_buttons = type & 0xF;
        extra = 0;
    }
    RECT rc;
    SetRect(&rc, CW_USEDEFAULT, CW_USEDEFAULT, cx, cy);
    if (!Window::create(parent, NULL, &rc, extra | 0x80C80000, 0x10101, title))
        return false;
    m_owner = Modal_getTopLevelWindow(parent);
    Window_centerOn(m_hWnd, m_owner);
    show(true);
    return true;
}

bool Modal::onCreate()
{
    RECT tmp;
    RECT rc = *getClientRect(&tmp);
    int y = rc.bottom - m_btnMarginBottom;
    int x = rc.right - m_btnMarginRight - m_btnWidth;
    m_btnX = x;
    int w;
    switch (m_buttons)
    {
    case 0:
    {
        HWND btn = CreateWindowExW(0, L"Button", getButtonText(1), 0x50000000, x, y, m_btnWidth, 24,
                                   m_hWnd, (HMENU)1, NULL, NULL);
        if (btn)
            SendMessageW(btn, WM_SETFONT, (WPARAM)getStdFont(0), 0);
        m_btnOk = btn;
        return true;
    }
    case 1:
        w = m_btnWidth;
        m_btnOk = createButton((HMENU)1, x - w - 20, y, w, 24, getButtonText(1), 0, 0);
        m_btnCancel = createButton((HMENU)2, x, y, m_btnWidth, 24, getButtonText(2), 0, 0);
        return true;
    case 2:
        w = m_btnWidth;
        m_btnAbort = createButton((HMENU)3, x - (2 * w + 40), y, w, 24, getButtonText(3), 0, 0);
        w = m_btnWidth;
        m_btnRetry = createButton((HMENU)4, x - w - 20, y, w, 24, getButtonText(4), 0, 0);
        m_btnIgnore = createButton((HMENU)5, x, y, m_btnWidth, 24, getButtonText(5), 0, 0);
        return true;
    case 3:
        w = m_btnWidth;
        m_btnCancel = createButton((HMENU)2, x - (2 * w + 40), y, w, 24, getButtonText(2), 0, 0);
        break;
    case 4:
        break;
    case 5:
        w = m_btnWidth;
        m_btnCancel = createButton((HMENU)2, x - w - 20, y, w, 24, getButtonText(2), 0, 0);
        m_btnRetry = createButton((HMENU)4, x, y, m_btnWidth, 24, getButtonText(4), 0, 0);
        return true;
    case 6:
        w = m_btnWidth;
        m_btnCancel = createButton((HMENU)2, x - (2 * w + 40), y, w, 24, getButtonText(2), 0, 0);
        w = m_btnWidth;
        m_btnRetry = createButton((HMENU)10, x - w - 20, y, w, 24, getButtonText(10), 0, 0);
        m_btnIgnore = createButton((HMENU)11, x, y, m_btnWidth, 24, getButtonText(11), 0, 0);
        return true;
    case 7:
        w = m_btnWidth;
        m_btnCancel = createButton((HMENU)2, (rc.right - w) / 2, y, w, 24, getButtonText(2), 0, 0);
        return true;
    default:
        return true;
    }
    w = m_btnWidth;
    m_btnNo = createButton((HMENU)7, x - w - 20, y, w, 24, getButtonText(7), 0, 0);
    m_btnYes = createButton((HMENU)6, x, y, m_btnWidth, 24, getButtonText(6), 0, 0);
    return true;
}

BOOL Modal::resize(int cx, int cy)
{
    RECT tmp;
    getClientRect(&tmp);
    SetWindowPos(m_hWnd, NULL, 0, 0, cx, cy, SWP_NOMOVE | SWP_NOZORDER);
    RECT tmp2;
    RECT rc = *getClientRect(&tmp2);
    int w = m_btnWidth;
    int y = rc.bottom - m_btnMarginBottom;
    int x = rc.right - m_btnMarginRight - w;
    if (!m_rightAlignButtons)
        x = m_btnX;
    switch (m_buttons)
    {
    case 0:
        return SetWindowPos(m_btnOk, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    case 1:
        SetWindowPos(m_btnOk, NULL, x - w - 20, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        return SetWindowPos(m_btnCancel, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    case 2:
        SetWindowPos(m_btnAbort, NULL, x - (2 * w + 40), y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        SetWindowPos(m_btnRetry, NULL, x - m_btnWidth - 20, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        return SetWindowPos(m_btnIgnore, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    case 3:
        SetWindowPos(m_btnCancel, NULL, x - (2 * w + 40), y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        SetWindowPos(m_btnNo, NULL, x - m_btnWidth - 20, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        return SetWindowPos(m_btnYes, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    case 4:
        SetWindowPos(m_btnNo, NULL, x - w - 20, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        return SetWindowPos(m_btnYes, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    case 5:
        Modal_moveWindow(m_btnCancel, x - w - 20, y);
        return Modal_moveWindow(m_btnRetry, x, y);
    case 6:
        Modal_moveWindow(m_btnCancel, x - (2 * w + 40), y);
        Modal_moveWindow(m_btnRetry, x - m_btnWidth - 20, y);
        return Modal_moveWindow(m_btnIgnore, x, y);
    case 7:
        return Modal_moveWindow(m_btnCancel, rc.right / 2 - 50, y);
    }
    return (BOOL)m_buttons;
}

LPCWSTR Modal::getButtonText(int id)
{
    switch (id)
    {
    case 1:  return L"Ok";
    case 2:  return L"Cancel";
    case 3:  return L"Abort";
    case 4:  return L"Retry";
    case 5:  return L"Ignore";
    case 6:  return L"Yes";
    case 7:  return L"No";
    case 10: return L"Try Again";
    case 11: return L"Continue";
    default: return L"<undefined>";
    }
}

bool Modal::onClose()
{
    if (mapCommandToResult(IDCLOSE) == -1)
        return false;
    if (!m_modelessOwner)
    {
        if (m_owner)
            EnableWindow(m_owner, TRUE);
        if (!m_result)
            m_result = IDCLOSE;
        m_running = false;
    }
    return true;
}

void Modal::onNcDestroy()
{
    if (m_modelessOwner)
        delete this;
}

bool Modal::onKey(WPARAM vk, LPARAM flags, bool down)
{
    if (!m_modelessOwner || !down)
        return Container::onKey(vk, flags, down);
    if ((short)vk == VK_RETURN)
    {
        PostMessageW(m_hWnd, WM_COMMAND, IDOK, (LPARAM)m_hWnd);
        return true;
    }
    if ((short)vk != VK_ESCAPE)
        return Container::onKey(vk, flags, down);
    PostMessageW(m_hWnd, WM_COMMAND, IDCANCEL, (LPARAM)m_hWnd);
    return true;
}

int Modal::doModal()
{
    HWND focus = GetFocus();
    if (m_owner)
        EnableWindow(m_owner, FALSE);
    m_running = true;
    g_currentModal = this;
    onShowDialog();
    SetCursor(g_arrowCursor);
    while (m_running)
    {
        MSG msg;
        glView* idle;
        if (PeekMessageW(&msg, NULL, 0, 0, PM_NOREMOVE) || (idle = g_activeGlView) == 0)
        {
            if (GetMessageW(&msg, NULL, 0, 0) && !IsDialogMessageW(m_hWnd, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }
        else
        {
            if (g_activeGlView->render())
                idle->swapBuffers();
            idle->idle();
        }
    }
    g_currentModal = 0;
    if (m_owner)
        EnableWindow(m_owner, TRUE);
    if (focus)
        SetFocus(focus);
    return m_result;
}

HWND Modal::showModeless(Window* owner)
{
    m_modelessOwner = owner;
    onShowDialog();
    SetForegroundWindow(m_hWnd);
    return SetActiveWindow(m_hWnd);
}
