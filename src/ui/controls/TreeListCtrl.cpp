#include "TreeListCtrl.h"
#include "TreeList.h"
#include "dragdrop.h"
#include "_imports_p10.h"

static NMLVKEYDOWN g_treeKeyDown;

const wchar_t* TreeListCtrl::getClassName()
{
    return L"SysTreeView32";
}

LPCWSTR TreeListCtrl::getThemeClass()
{
    return L"TREEVIEW";
}

DWORD TreeListCtrl::getStyle()
{
    return 0x50001007;
}

bool TreeListCtrl::onDropFile(int index, POINT* pt, const wchar_t* path)
{
    return m_owner->onCtrlDropFile(index, pt, path);
}

LPRECT TreeListCtrl::getItemRect(LPRECT out, HTREEITEM item)
{
    RECT label;
    SetRectEmpty(out);
    SetRectEmpty(&label);
    *(HTREEITEM*)&label = item;
    SendMessageW(m_hWnd, TVM_GETITEMRECT, TRUE, (LPARAM)&label);
    *(HTREEITEM*)out = item;
    SendMessageW(m_hWnd, TVM_GETITEMRECT, FALSE, (LPARAM)out);
    out->left = label.left;
    return out;
}

bool TreeListCtrl::onLButton(int keys, POINT* pt, bool down)
{
    m_flags |= 0x10;
    if (!down)
        return false;
    if (pt->x > m_totalWidth)
    {
        SendMessageW(m_hWnd, TVM_SELECTITEM, TVGN_CARET, 0);
        return false;
    }
    TVHITTESTINFO ht;
    ht.pt = *pt;
    ht.flags = 0;
    ht.hItem = 0;
    LRESULT hit = SendMessageW(m_hWnd, TVM_HITTEST, 0, (LPARAM)&ht);
    if (!hit)
    {
        m_owner->clearSelection(true);
        return false;
    }
    LPARAM xy = (WORD)pt->x | ((WORD)pt->y << 16);
    if (keys & MK_CONTROL)
    {
        bool select = SendMessageW(m_hWnd, TVM_GETITEMSTATE, (WPARAM)ht.hItem, TVIS_SELECTED) != TVIS_SELECTED;
        HTREEITEM caret = (HTREEITEM)SendMessageW(m_hWnd, TVM_GETNEXTITEM, TVGN_CARET, 0);
        LRESULT caretState;
        if (caret)
            caretState = SendMessageW(m_hWnd, TVM_GETITEMSTATE, (WPARAM)caret, TVIS_SELECTED);
        else
            caretState = 0;
        DefSubclassProc(m_hWnd, WM_LBUTTONUP, keys, xy);
        m_owner->setItemSelected(ht.hItem, select);
        if (caret && caret != ht.hItem)
            m_owner->setItemSelected(caret, caretState == TVIS_SELECTED);
        m_owner->m_anchor = 0;
        return true;
    }
    if (keys & MK_SHIFT)
    {
        if (m_owner->canMultiSelect(ht.hItem))
        {
            if (!m_owner->m_anchor)
                m_owner->m_anchor = (HTREEITEM)SendMessageW(m_hWnd, TVM_GETNEXTITEM, TVGN_CARET, 0);
            DefSubclassProc(m_hWnd, WM_LBUTTONUP, keys, xy);
            if (m_owner->m_anchor)
                m_owner->selectRange(m_owner->m_anchor, ht.hItem);
        }
        else if (m_owner->m_anchor)
        {
            m_owner->clearSelection(true);
            return true;
        }
        return true;
    }
    if ((ht.flags & TVHT_ONITEMBUTTON) != 0 || (ht.flags & (TVHT_ONITEMLABEL | TVHT_ONITEMRIGHT)) == 0)
        return false;
    m_owner->clearSelection(true);
    m_owner->setItemSelected(ht.hItem, false);
    m_owner->setItemSelected(ht.hItem, true);
    SendMessageW(m_hWnd, TVM_SELECTITEM, TVGN_CARET, hit);
    m_owner->m_anchor = ht.hItem;
    return true;
}

bool TreeListCtrl::onRButton(int keys, POINT* pt, bool down)
{
    if (!down)
        return false;
    TVHITTESTINFO ht;
    ht.pt = *pt;
    ht.flags = 0;
    ht.hItem = 0;
    LRESULT item = SendMessageW(m_hWnd, TVM_HITTEST, 0, (LPARAM)&ht);
    if (!item)
        return false;
    if (item != SendMessageW(m_hWnd, TVM_GETNEXTITEM, TVGN_CARET, 0))
        SendMessageW(m_hWnd, TVM_SELECTITEM, TVGN_CARET, item);
    HWND parent = GetParent(m_hWnd);
    LRESULT menuId = SendMessageW(parent, WM_USER + 7, 0x65, item);
    if (menuId <= 0)
        return false;
    HMENU menu = LoadMenuW(0, MAKEINTRESOURCEW((WORD)menuId));
    if (!menu)
        return false;
    HMENU popup = GetSubMenu(menu, 0);
    if (popup)
    {
        SetMenuDefaultItem(popup, (UINT)-1, TRUE);
        SendMessageW(parent, WM_USER + 8, (WPARAM)popup, item);
        ClientToScreen(m_hWnd, pt);
        TrackPopupMenu(popup, 0, pt->x, pt->y, 0, parent, 0);
    }
    DestroyMenu(menu);
    return true;
}

bool TreeListCtrl::onDoubleClick(int keys, POINT* pt, int button)
{
    if (button)
        return false;
    if (pt->x >= m_totalWidth)
        return false;
    TVHITTESTINFO ht;
    ht.pt = *pt;
    ht.flags = 0;
    ht.hItem = 0;
    LRESULT item = SendMessageW(m_hWnd, TVM_HITTEST, 0, (LPARAM)&ht);
    if (!item)
        return false;
    PostMessageW(GetParent(m_hWnd), WM_USER + 11, keys, item);
    return true;
}

bool TreeListCtrl::onKey(short vk, int flags, bool down)
{
    short key = (short)vk;
    if (key == VK_ESCAPE && g_dragImage)
    {
        endDragImage();
        SendMessageW(m_hWnd, TVM_SELECTITEM, TVGN_DROPHILITE, 0);
        return true;
    }
    if (!down)
        return false;
    if (key != VK_RETURN && key != VK_DELETE)
    {
        if (GetKeyState(VK_CONTROL) < 0 && (key == VK_PRIOR || key == VK_NEXT))
        {
            HWND grandParent = GetParent(GetParent(m_owner->m_hWnd));
            if (grandParent)
            {
                memset(&g_treeKeyDown, 0, sizeof(g_treeKeyDown));
                g_treeKeyDown.hdr.code = (UINT)-155;
                g_treeKeyDown.hdr.hwndFrom = m_hWnd;
                g_treeKeyDown.hdr.idFrom = (UINT_PTR)m_id;
                g_treeKeyDown.wVKey = (WORD)key;
                PostMessageW(grandParent, WM_NOTIFY, 0, (LPARAM)&g_treeKeyDown);
            }
        }
        return false;
    }
    LRESULT item = SendMessageW(m_hWnd, TVM_GETNEXTITEM, TVGN_CARET, 0);
    if (!item)
        return false;
    UINT msg;
    WPARAM wParam;
    LPARAM lParam;
    if (key == VK_RETURN)
    {
        lParam = item;
        wParam = (WPARAM)flags;
        msg = WM_USER + 11;
    }
    else
    {
        lParam = 0;
        wParam = 0xE12B;
        msg = WM_COMMAND;
    }
    PostMessageW(GetParent(m_hWnd), msg, wParam, lParam);
    return true;
}

bool TreeListCtrl::onMouseMove(int keys, POINT* pt)
{
    if (!m_tracking)
    {
        TRACKMOUSEEVENT track;
        track.hwndTrack = m_hWnd;
        track.cbSize = 16;
        track.dwFlags = TME_LEAVE;
        track.dwHoverTime = HOVER_DEFAULT;
        m_tracking = TrackMouseEvent(&track) != 0;
    }
    RECT rc;
    LRESULT item = 0;
    if (pt->x < m_totalWidth)
    {
        TVHITTESTINFO ht;
        ht.pt = *pt;
        ht.flags = 0;
        ht.hItem = 0;
        item = SendMessageW(m_hWnd, TVM_HITTEST, 0, (LPARAM)&ht);
    }
    if (item)
    {
        HTREEITEM old = m_hotItem;
        if (old != (HTREEITEM)item)
        {
            if (old)
                InvalidateRect(m_hWnd, getItemRect(&rc, old), TRUE);
            m_hotItem = (HTREEITEM)item;
            InvalidateRect(m_hWnd, getItemRect(&rc, (HTREEITEM)item), FALSE);
        }
    }
    else
    {
        if (m_hotItem)
        {
            HTREEITEM old = m_hotItem;
            BOOL erase = SendMessageW(m_hWnd, TVM_GETNEXTITEM, TVGN_CARET, 0) != (LRESULT)old;
            InvalidateRect(m_hWnd, getItemRect(&rc, old), erase);
        }
        m_hotItem = 0;
    }
    return false;
}

bool TreeListCtrl::onMouseLeave()
{
    bool none = m_hotItem == 0;
    m_tracking = false;
    if (!none)
    {
        if (IsWindow(m_hWnd))
        {
            RECT rc;
            InvalidateRect(m_hWnd, getItemRect(&rc, m_hotItem), TRUE);
        }
        m_hotItem = 0;
    }
    return true;
}

LRESULT TreeListCtrl::onCustomDraw(NMLVCUSTOMDRAW* cd, int ctrlId)
{
    return m_owner->onCustomDraw((NMTVCUSTOMDRAW*)cd);
}
