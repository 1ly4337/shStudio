#include "TreeList.h"
#include <cwchar>
#include <cstdio>
#include "Cell.h"
#include "Node.h"
#include "ToggleNode.h"
#include "oStream.hpp"
#include "iStream.hpp"
#include "Exception.h"
#include "GDC.h"
#include "WindowDC.h"
#include "Clipboard.h"
#include "DropSource.h"
#include "dragdrop.h"
#include "Document.h"
#include "p10_vcall.h"
#include "gdi_cache.h"
#include "_imports_p10.h"

static int s_headerHeight = 0;
static int s_cyHScroll = GetSystemMetrics(SM_CYHSCROLL);
static int s_cxVScroll = GetSystemMetrics(SM_CXVSCROLL);
static UINT s_drawState;
static RECT s_drawRect;

template <class T>
static void pvClear(primitiveVector<T>& v)
{
    for (int i = 0; i < v.m_capacity; ++i)
        v.resetItem(&v.m_items[i]);
    v.m_count = 0;
}

template <class T>
static bool pvRemove(primitiveVector<T>& v, T value)
{
    int count = v.m_count;
    int index = 0;
    if (count <= 0)
        return false;
    for (T* p = v.m_items; *p != value; ++p)
    {
        if (++index >= count)
            return false;
    }
    if (index < 0 || index >= count)
        return false;
    v.resetItem(&v.m_items[index]);
    for (int j = index + 1; j < v.m_count; ++j)
        v.m_items[j - 1] = v.m_items[j];
    --v.m_count;
    v.m_items[v.m_count] = 0;
    return true;
}

template <class T>
static bool pvContains(const primitiveVector<T>& v, T value)
{
    int count = v.m_count;
    int index = 0;
    if (count <= 0)
        return false;
    for (T* p = v.m_items; *p != value; ++p)
    {
        if (++index >= count)
            return false;
    }
    return true;
}

static inline HWND TreeList_treeHwnd(TreeList* t) { return FIELD(HWND, &t->m_tree, 0x04); }

TreeList::TreeList()
    : m_tree(this),
      m_hHeader(0), m_hScroll(0), m_64(0), m_anchor(0),
      m_dropTarget(0), m_columnCount(0), m_totalWidth(0), m_8C(0x40000),
      m_treeBottom(0), m_scrollX(0), m_lastWidth(0), m_trackDC(0)
{
}

TreeList::~TreeList()
{
    if (m_dropTarget)
    {
        m_dropTarget->Release();
        m_dropTarget = 0;
    }
}

bool TreeList::onCreate()
{
    m_hHeader = CreateWindowExW(0, L"SysHeader32", 0, WS_CHILD | HDS_FULLDRAG | HDS_DRAGDROP | HDS_HOTTRACK | HDS_BUTTONS,

                                0, 0, 0, 0, m_hWnd, (HMENU)0x66, hModule, 0);
    if (!m_hHeader)
        return false;
    SendMessageW(m_hHeader, WM_SETFONT, (WPARAM)getStdFont(0), 1);
    SIZE size;
    getSize(&size);
    int width = size.cx;
    int height = size.cy;
    if (!s_headerHeight)
    {
        RECT rc;
        WINDOWPOS wp;
        HDLAYOUT layout;
        wp.hwnd = 0;
        layout.prc = &rc;
        layout.pwpos = &wp;
        wp.hwndInsertAfter = 0; wp.x = 0; wp.y = 0; wp.cx = 0; wp.cy = 0; wp.flags = 0;
        GetClientRect(m_hWnd, &rc);
        if (!SendMessageW(m_hHeader, HDM_LAYOUT, 0, (LPARAM)&layout))
            return false;
        s_headerHeight = wp.cy;
    }
    SetWindowPos(m_hHeader, 0, 0, 0, width, s_headerHeight, SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_NOZORDER);
    height -= s_headerHeight;
    if (!m_tree.Control::create(m_hWnd, (HMENU)0x65, 0, s_headerHeight, width, height, 0x20, 0, s_headerHeight))
        return false;
    m_hScroll = CreateWindowExW(0, L"ScrollBar", 0, WS_CHILD, 0, height, width - s_cxVScroll, s_cyHScroll,
                                m_hWnd, (HMENU)0x67, hModule, 0);
    return m_hScroll != 0;
}

void TreeList::onDestroy()
{
    HIMAGELIST list = (HIMAGELIST)SendMessageW(TreeList_treeHwnd(this), TVM_GETIMAGELIST, TVSIL_NORMAL, 0);
    if (list)
        ImageList_Destroy(list);
    if (m_dropTarget)
    {
        m_dropTarget->Release();
        m_dropTarget = 0;
    }
}

bool TreeList::onCommand(UINT code, HWND ctrl, UINT id)
{
    if ((code | 1) != 1 || ctrl)
        return false;
    switch (id)
    {
    case 0xE122: onCmdE122();      return true;
    case 0xE123: copySelected();   return true;
    case 0xE124: pasteSelected();  return true;
    case 0xE12A: editSelected();   return true;
    case 0xE12B: deleteSelected(); return true;
    }
    if (id != 0xE12C)
        return false;
    updateSelected();
    return true;
}

bool TreeList::onItemActivate(HTREEITEM item, WPARAM keys)
{
    Node* node = getItemNode(item);
    if (!dynamic_cast<ToggleNode*>(node))
        return false;
    SendMessageW(TreeList_treeHwnd(this), TVM_EXPAND, TVE_TOGGLE, (LPARAM)item);
    return true;
}

void TreeList::onFocus(bool focused)
{
    if (focused)
        SetFocus(TreeList_treeHwnd(this));
}

void TreeList::onActivate(bool active)
{
    if (active)
        SetFocus(TreeList_treeHwnd(this));
}

bool TreeList::insertColumn(const LVCOLUMNW* column, int id)
{
    if (!IsWindow(m_hHeader))
        return false;
    HDITEMW hdi;
    memset(&hdi, 0, sizeof(hdi));
    UINT mask = 0;
    UINT colMask = column->mask;
    if (colMask & LVCF_WIDTH)
    {
        mask |= HDI_WIDTH;
        hdi.cxy = column->cx;
    }
    if (colMask & LVCF_TEXT)
    {
        mask |= HDI_TEXT;
        hdi.pszText = column->pszText;
    }
    if (colMask & LVCF_FMT)
    {
        mask |= HDI_FORMAT;
        hdi.fmt = column->fmt & 3;
    }
    int columnId = id;
    if (id == -1)
        columnId = m_columnCount + 1;
    mask |= HDI_LPARAM;
    hdi.mask = mask;
    hdi.lParam = columnId;
    if (colMask & LVCF_IMAGE)
    {
        hdi.mask = mask | HDI_IMAGE;
        hdi.iImage = column->iImage;
    }
    if (SendMessageW(m_hHeader, HDM_INSERTITEMW, m_columnCount, (LPARAM)&hdi) == -1)
        return false;
    m_columnIds.add(columnId);
    m_columnFormats.add((unsigned short)column->fmt);
    m_columnWidths.add(hdi.cxy);
    m_totalWidth += column->cx;
    ++m_columnCount;
    m_tree.m_totalWidth = m_totalWidth;
    return true;
}

LRESULT TreeList::deleteAllItems()
{
    HWND tree = TreeList_treeHwnd(this);
    m_64 = 0;
    return SendMessageW(tree, TVM_DELETEITEM, 0, (LPARAM)TVI_ROOT);
}

HTREEITEM TreeList::insertItem(HTREEITEM parent, LPCWSTR text, int image, int children, HTREEITEM after)
{
    if (parent == TVI_ROOT)
        after = TVI_ROOT;
    TVINSERTSTRUCTW tvis;
    memset(&tvis, 0, sizeof(tvis));
    tvis.item.pszText = (LPWSTR)text;
    tvis.item.iImage = image;
    tvis.item.iSelectedImage = image;
    tvis.hParent = parent;
    tvis.hInsertAfter = after;
    tvis.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
    if (children > 0)
    {
        tvis.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN;
        tvis.item.cChildren = children;
    }
    return (HTREEITEM)SendMessageW(TreeList_treeHwnd(this), TVM_INSERTITEMW, 0, (LPARAM)&tvis);
}

HTREEITEM TreeList::insertNode(HTREEITEM parent, Node* node, int image, int children, HTREEITEM after)
{
    if (!node)
        return 0;
    Cell* cell = new Cell(node);
    return insertCell(cell, parent, image, children, after);
}

HTREEITEM TreeList::insertCell(Cell* cell, HTREEITEM parent, int image, int children, HTREEITEM after)
{
    if (!cell)
        return 0;
    if (parent == TVI_ROOT)
        after = TVI_ROOT;
    TVINSERTSTRUCTW tvis;
    memset(&tvis, 0, sizeof(tvis));
    UINT mask = TVIF_TEXT | TVIF_PARAM;
    tvis.hParent = parent;
    tvis.hInsertAfter = after;
    tvis.item.mask = TVIF_TEXT | TVIF_PARAM;
    tvis.item.lParam = (LPARAM)cell;
    tvis.item.pszText = LPSTR_TEXTCALLBACKW;
    if (image != -2)
    {
        mask = TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE;
        tvis.item.mask = mask;
        tvis.item.iImage = image;
        tvis.item.iSelectedImage = image;
    }
    if (children)
    {
        tvis.item.mask = mask | TVIF_CHILDREN;
        tvis.item.cChildren = children;
    }
    HTREEITEM item = (HTREEITEM)SendMessageW(TreeList_treeHwnd(this), TVM_INSERTITEMW, 0, (LPARAM)&tvis);
    cell->m_node->m_data = (int)(INT_PTR)item;
    return item;
}

bool TreeList::deleteChildren(HTREEITEM item)
{
    if (!item)
        return false;
    for (LRESULT child = SendMessageW(TreeList_treeHwnd(this), TVM_GETNEXTITEM, TVGN_CHILD, (LPARAM)item); child;
         child = SendMessageW(TreeList_treeHwnd(this), TVM_GETNEXTITEM, TVGN_CHILD, (LPARAM)item))
    {
        SendMessageW(TreeList_treeHwnd(this), TVM_DELETEITEM, 0, child);
    }
    onItemsChanged();
    return true;
}

Cell* TreeList::getItemCell(HTREEITEM item)
{
    if (!item)
        return 0;
    TVITEMEXW tvi;
    memset(&tvi, 0, sizeof(tvi));
    tvi.mask = TVIF_HANDLE | TVIF_PARAM;
    tvi.hItem = item;
    LRESULT ok = SendMessageW(TreeList_treeHwnd(this), TVM_GETITEMW, 0, (LPARAM)&tvi);
    return ok != 0 ? (Cell*)tvi.lParam : 0;
}

Node* TreeList::getItemNode(HTREEITEM item)
{
    Cell* cell;
    if (item && (cell = getItemCell(item)) != 0)
        return cell->m_node;
    return 0;
}

String TreeList::getItemText(HTREEITEM item)
{
    wchar_t text[256 + 2];
    memset(text, 0, 0x200);
    TVITEMEXW tvi;
    memset(&tvi, 0, sizeof(tvi));
    tvi.pszText = text;
    tvi.mask = TVIF_TEXT;
    tvi.hItem = item;
    tvi.cchTextMax = 256;
    SendMessageW(TreeList_treeHwnd(this), TVM_GETITEMW, 0, (LPARAM)&tvi);
    return String(text, (unsigned)-1);
}

BOOL TreeList::setItemCut(HTREEITEM item, UINT state)
{
    TVITEMW tvi;
    tvi.state = state;
    tvi.mask = TVIF_STATE;
    tvi.hItem = item;
    tvi.stateMask = TVIS_CUT;
    SendMessageW(TreeList_treeHwnd(this), TVM_SETITEMW, 0, (LPARAM)&tvi);
    RECT rc;
    return InvalidateRect(TreeList_treeHwnd(this), m_tree.getItemRect(&rc, item), FALSE);
}

HRESULT TreeList::onGetDispInfo(TVITEMW* item)
{
    Cell* cell = (Cell*)item->lParam;
    item->pszText = 0;
    item->iImage = 0;
    item->iSelectedImage = 0;
    if (!cell)
        return E_POINTER;
    Node* node = cell->m_node;
    if (!node)
        return E_POINTER;
    if (item->mask & TVIF_TEXT)
    {
        if (m_columnIds.m_count <= 0)
            throw ArrayIndexOutOfBoundsException(0, m_columnIds.m_count);
        item->pszText = (LPWSTR)node->getString((DWORD)m_columnIds.m_items[0]);
        if (!item->pszText)
            item->pszText = (LPWSTR)L"";
    }
    int image = node->getId();
    if (image < 0)
        image = 0;
    UINT mask = item->mask;
    if (mask & (TVIF_IMAGE | TVIF_SELECTEDIMAGE))
        item->iImage = image;
    if (mask & TVIF_SELECTEDIMAGE)
        item->iSelectedImage = image;
    if (mask & TVIF_CHILDREN)
        item->cChildren = node->getInt('chlc');
    return S_OK;
}

int TreeList::recalcColumnWidths()
{
    m_columnWidths.m_count = 0;
    for (int i = 0; i < m_columnCount; ++i)
    {
        HDITEMW hdi;
        memset(&hdi, 0, sizeof(hdi));
        hdi.mask = HDI_WIDTH;
        SendMessageW(m_hHeader, HDM_GETITEMW, i, (LPARAM)&hdi);
        m_columnWidths.add(hdi.cxy);
        m_totalWidth += hdi.cxy;
    }
    m_tree.m_totalWidth = m_totalWidth;
    return m_totalWidth;
}

bool TreeList::onNotify(NMHDR* hdr, LRESULT* result)
{
    UINT_PTR from = hdr->idFrom;
    if (from == 101)
    {
        switch ((int)hdr->code)
        {
        case NM_CUSTOMDRAW:
            if (IsWindowVisible(TreeList_treeHwnd(this)))
                *result = onCustomDraw((NMTVCUSTOMDRAW*)hdr);
            return true;
        case TVN_SELCHANGEDW:
        {
            NMTREEVIEWW* nm = (NMTREEVIEWW*)hdr;
            Node* oldNode = nm->itemOld.lParam ? ((Cell*)nm->itemOld.lParam)->m_node : 0;
            Node* newNode = nm->itemNew.lParam ? ((Cell*)nm->itemNew.lParam)->m_node : 0;
            onSelChanged(oldNode, newNode);
            pvClear(m_selection);
            m_selection.add(nm->itemNew.hItem);
            return true;
        }
        case TVN_GETDISPINFOW:
            if (IsWindowVisible(TreeList_treeHwnd(this)))
            {
                onGetDispInfo(&((NMTVDISPINFOW*)hdr)->item);
                return true;
            }
            return false;
        case TVN_DELETEITEMW:
        {
            Cell* cell = (Cell*)((NMTREEVIEWW*)hdr)->itemOld.lParam;
            if (cell)
                delete cell;
            return true;
        }
        }
        return false;
    }
    if (from != 102)
        return false;
    POINT pt;
    pt.x = 0;
    pt.y = 0;
    GetCursorPos(&pt);
    ScreenToClient(m_hWnd, &pt);
    switch ((int)hdr->code)
    {
    case HDN_BEGINTRACKW:
    {
        m_trackX = pt.x;
        m_trackDC = new WindowDC(m_hWnd);
        SetROP2(m_trackDC->m_hdc, R2_NOT);
        WindowDC* dc = m_trackDC;
        HGDIOBJ old = SelectObject(dc->m_hdc, CreatePen(PS_DOT, 1, 0));
        if (dc->m_savedPen)
            DeleteObject(old);
        else
            dc->m_savedPen = old;
        int bottom = m_treeBottom;
        int x = m_trackX;
        MoveToEx(m_trackDC->m_hdc, x, s_headerHeight, 0);
        LineTo(m_trackDC->m_hdc, x, bottom);
        return true;
    }
    case HDN_ENDTRACKW:
        if (m_trackDC)
        {
            delete m_trackDC;
            m_trackDC = 0;
        }
        m_totalWidth = 0;
        m_columnWidths.m_count = 0;
        PostMessageW(m_hWnd, WM_SIZE, 0, 0);
        return true;
    case HDN_TRACKW:
        if (m_trackDC)
        {
            m_trackDC->GDC::line(m_trackX, s_headerHeight, m_trackX, m_treeBottom);
            int bottom = m_treeBottom;
            m_trackX = pt.x;
            m_trackDC->GDC::line(pt.x, s_headerHeight, pt.x, bottom);
        }
        return true;
    default:
        return false;
    }
}

bool TreeList::onScroll(bool horizontal, int code, int pos)
{
    if (!horizontal)
        return false;
    int total = m_totalWidth;
    int x = m_scrollX;
    SIZE size;
    int page = getSize(&size)->cx;
    if (GetWindowLongW(TreeList_treeHwnd(this), GWL_STYLE) & WS_VSCROLL)
        total += s_cxVScroll;
    switch (code)
    {
    case SB_LINELEFT:
        x -= 12;
        goto clampLow;
    case SB_LINERIGHT:
        x += 12;
        goto clampLow;
    case SB_PAGELEFT:
        x -= page;
        goto clampLow;
    case SB_PAGERIGHT:
        x += page;
        goto clampLow;
    case SB_THUMBTRACK:
        x = 12 * ((pos + 11) / 12);
        goto clampLow;
    case SB_LEFT:
        x = 0;
        goto clampHigh;
    case SB_RIGHT:
        x = total;
        goto clampLow;
    case SB_ENDSCROLL:
        InvalidateRect(m_hWnd, 0, TRUE);
        return true;
    default:
        goto clampLow;
    }
clampLow:
    if (x < 0)
        x = 0;
clampHigh:
    if (x > total - page)
        x = total - page;
    SetScrollPos(m_hScroll, SB_CTL, x, TRUE);
    int height = s_headerHeight;
    int delta = x - m_scrollX;
    m_scrollX = x;
    SetWindowPos(m_hHeader, 0, -x, 0, x + page, height, SWP_NOZORDER);
    ScrollWindowEx(TreeList_treeHwnd(this), -delta, 0, 0, 0, 0, 0, SW_INVALIDATE | SW_ERASE);
    return true;
}

void TreeList::onSize(WPARAM type, int cx, int cy)
{
    UINT flags = SWP_NOZORDER;
    if (!m_totalWidth)
    {
        recalcColumnWidths();
        flags = SWP_NOZORDER | SWP_FRAMECHANGED;
    }
    SIZE size;
    getSize(&size);
    int width = size.cx;
    int height = size.cy;
    int total = m_totalWidth;
    if (GetWindowLongW(TreeList_treeHwnd(this), GWL_STYLE) & WS_VSCROLL)
        total += s_cxVScroll;
    if (width >= total)
    {
        if (IsWindowVisible(m_hScroll))
        {
            ShowWindow(m_hScroll, SW_HIDE);
            if (m_scrollX > 0)
            {
                flags |= SWP_FRAMECHANGED;
                m_scrollX = 0;
            }
        }
    }
    else
    {
        if (!IsWindowVisible(m_hScroll))
            ShowWindow(m_hScroll, SW_SHOW);
        if (m_lastWidth < width && m_scrollX > 0)
        {
            flags |= SWP_FRAMECHANGED;
            m_scrollX = m_lastWidth + m_scrollX - width;
        }
        SCROLLINFO si;
        si.cbSize = sizeof(si);
        si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
        si.nMin = 0;
        si.nMax = total;
        si.nPage = width;
        si.nPos = m_scrollX;
        si.nTrackPos = 0;
        SetScrollInfo(m_hScroll, SB_CTL, &si, FALSE);
    }
    m_lastWidth = width;
    if (IsWindowVisible(m_hScroll))
        height -= s_cyHScroll;
    m_treeBottom = height;
    SetWindowPos(m_hHeader, 0, -m_scrollX, 0, m_scrollX + width, s_headerHeight, flags | SWP_NOZORDER | SWP_NOACTIVATE);
    SetWindowPos(TreeList_treeHwnd(this), 0, 0, s_headerHeight, width, height - s_headerHeight,
                 flags | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE);
    int vscroll = 0;
    if (GetWindowLongW(TreeList_treeHwnd(this), GWL_STYLE) & WS_VSCROLL)
        vscroll = s_cxVScroll;
    SetWindowPos(m_hScroll, 0, 0, height, width - vscroll, s_cyHScroll, SWP_NOZORDER | SWP_NOACTIVATE);
    if (flags & SWP_FRAMECHANGED)
        InvalidateRect(m_hWnd, 0, TRUE);
}

LRESULT TreeList::defWindowProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg != WM_NCPAINT)
    {
        switch (msg)
        {
        case WM_GETDLGCODE:
            return DLGC_WANTARROWS | DLGC_WANTCHARS;
        case 0x407:
            if (wParam == 101)
                return getItemMenu((HTREEITEM)lParam);
            break;
        case 0x408:
            if (wParam)
                onInitItemMenu((HMENU)wParam, TreeList_treeHwnd(this), (HTREEITEM)lParam);
            break;
        case 0x40B:
            onItemActivate((HTREEITEM)lParam, wParam);
            return 0;
        default:
            return DefWindowProcW(m_hWnd, msg, wParam, lParam);
        }
        return 0;
    }
    LRESULT result = DefWindowProcW(m_hWnd, WM_NCPAINT, wParam, lParam);
    if (GetWindowLongW(m_hWnd, GWL_STYLE) & WS_BORDER)
    {
        WindowDC dc(m_hWnd);
        SIZE size;
        SIZE* s = getSize(&size);
        int cx = s->cx;
        int cy = s->cy;
        dc.m_savedPen = SelectObject(dc.m_hdc, CreatePen(PS_SOLID, 1, 0xB99D7F));
        --cy;
        MoveToEx(dc.m_hdc, 0, 0, 0);
        LineTo(dc.m_hdc, cx - 1, 0);
        LineTo(dc.m_hdc, cx - 1, cy);
        LineTo(dc.m_hdc, 0, cy);
        LineTo(dc.m_hdc, 0, 0);
    }
    return result;
}

LRESULT TreeList::onCustomDraw(NMTVCUSTOMDRAW* cd)
{
    switch (cd->nmcd.dwDrawStage)
    {
    case CDDS_PREPAINT:
        SetViewportOrgEx(cd->nmcd.hdc, -m_scrollX, 0, 0);
        return CDRF_NOTIFYITEMDRAW;

    case CDDS_ITEMPREPAINT:
    {
        if (IsRectEmpty(&cd->nmcd.rc))
            return CDRF_DODEFAULT;
        HTREEITEM item = (HTREEITEM)cd->nmcd.dwItemSpec;
        cd->clrText = GetSysColor(COLOR_WINDOW);
        COLORREF bk = GetSysColor(COLOR_WINDOW);
        cd->nmcd.uItemState &= ~CDIS_FOCUS;
        cd->clrTextBk = bk;
        LRESULT state = SendMessageW(TreeList_treeHwnd(this), TVM_GETITEMSTATE, (WPARAM)item, -1);
        s_drawRect.left = (LONG)(INT_PTR)item;
        s_drawState = (UINT)state;
        SendMessageW(TreeList_treeHwnd(this), TVM_GETITEMRECT, TRUE, (LPARAM)&s_drawRect);
        s_drawRect.right = m_totalWidth - 5;
        return CDRF_NOTIFYPOSTPAINT;
    }

    case CDDS_ITEMPOSTPAINT:
    {
        GDC dc(cd->nmcd.hdc);
        HTREEITEM item = (HTREEITEM)cd->nmcd.dwItemSpec;
        wchar_t text[256 + 2];
        memset(text, 0, 0x200);
        TVITEMEXW tvi;
        memset(&tvi, 0, sizeof(tvi));
        tvi.pszText = text;
        tvi.hItem = item;
        tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM;
        tvi.cchTextMax = 256;
        if (!SendMessageW(TreeList_treeHwnd(this), TVM_GETITEMW, 0, (LPARAM)&tvi))
            return CDRF_DODEFAULT;
        SetBkMode(dc.m_hdc, TRANSPARENT);
        SetTextColor(dc.m_hdc, GetSysColor(COLOR_WINDOWTEXT));

        int colorIndex;
        bool frame = false;
        if (s_drawState & TVIS_DROPHILITED)
        {
            colorIndex = 104;
            frame = true;
        }
        else if (pvContains(m_selection, item))
        {
            colorIndex = 101;
            if ((FIELD(BYTE, &m_tree, 0x0C) & 0x10) == 0)
                colorIndex = (GetWindowLongW(TreeList_treeHwnd(this), GWL_STYLE) & TVS_SHOWSELALWAYS) != 0 ? 103 : 0;
            if (item == m_tree.m_hotItem)
            {
                colorIndex = 102;
                frame = true;
            }
            else if (colorIndex)
            {
                frame = true;
            }
        }
        else if (item == m_tree.m_hotItem)
        {
            colorIndex = 100;
            frame = true;
        }
        if (frame)
        {
            COLORREF pen = getStdColor(colorIndex);
            dc.GDC::selectPen(pen, 1, PS_SOLID);
            HBRUSH brush = getSolidBrush(getStdColor(colorIndex + 10));
            dc.m_saved14 = SelectObject(dc.m_hdc, brush);
            RoundRect(dc.m_hdc, s_drawRect.left, s_drawRect.top, s_drawRect.right, s_drawRect.bottom, 5, 5);
        }

        RECT rc;
        CopyRect(&rc, &s_drawRect);
        rc.left += 4;
        if (!tvi.lParam)
        {
            if (m_columnFormats.m_count <= 0)
                throw ArrayIndexOutOfBoundsException((long)tvi.lParam, m_columnFormats.m_count);
            DrawTextW(dc.m_hdc, text, -1, &rc, (m_columnFormats.m_items[0] & 3) | DT_VCENTER | DT_SINGLELINE);
            return CDRF_SKIPDEFAULT;
        }

        Node* node = ((Cell*)tvi.lParam)->m_node;
        int x = 0;
        bool singleColumn = node->getInt('1col') == 1;
        for (int i = 0; i < m_columnCount; ++i)
        {
            if (i)
            {
                rc.left = x + 4;
                x += *m_columnWidths.at(i);
                rc.right = x - 4;
            }
            else
            {
                if (m_columnWidths.m_count <= 0)
                    throw ArrayIndexOutOfBoundsException(0, m_columnWidths.m_count);
                x = m_columnWidths.m_items[0];
                if (!singleColumn)
                    rc.right = m_columnWidths.m_items[0];
                drawFirstColumn(&dc, node, &rc);
            }
            memset(text, 0, 0x200);
            int columnId = *m_columnIds.at(i);
            short format = (short)*m_columnFormats.at(i);
            if (format == -1)
                format = node->getImage(columnId, -1);
            UINT align = format & 3;
            if (align)
                align = 2 - (align != 1);
            int type = format & 0xFF0;
            const wchar_t* str;
            if (type == 0x10)
            {
                int value = node->getInt((DWORD)columnId);
                if (value != -1)
                    swprintf_s(text, 0x100, L"%lu", value);
                str = text;
            }
            else if (type == 0x40)
            {
                int value = node->getInt((DWORD)columnId);
                swprintf_s(text, 0x100, L"0x%lX", value);
                str = text;
            }
            else if (type == 0x80)
            {
                int bits = node->getInt((DWORD)columnId);
                float value;
                memcpy(&value, &bits, sizeof(value));
                swprintf_s(text, 0x100, L"%.3f", (double)value);
                str = text;
            }
            else
            {
                str = node->getString((DWORD)columnId);
                if (!str)
                    continue;
            }
            DrawTextW(dc.m_hdc, str, -1, &rc, align | DT_VCENTER | DT_SINGLELINE);
            if (singleColumn)
                break;
        }
        return CDRF_SKIPDEFAULT;
    }
    }
    return CDRF_DODEFAULT;
}

bool TreeList::copySelected()
{
    LRESULT item = SendMessageW(TreeList_treeHwnd(this), TVM_GETNEXTITEM, TVGN_CARET, 0);
    Node* node = getItemNode((HTREEITEM)item);
    if (!node)
        return false;
    Streamable* object = node;
    int format = object->getVersion();
    if (format < 512)
        return false;
    oStream out(false, 1200);
    bool ok;
    if (object->writeClip(out))
    {
        String text;
        object->toText(text, 'csv ');
        if (Clipboard::setData((UINT)format, out, true))
            ok = Clipboard::setText(text.m_buffer, false);
        else
            ok = false;
    }
    else
    {
        ok = false;
    }
    return ok;
}

bool TreeList::pasteSelected()
{
    LRESULT item = SendMessageW(TreeList_treeHwnd(this), TVM_GETNEXTITEM, TVGN_CARET, 0);
    Node* node = getItemNode((HTREEITEM)item);
    if (!node)
        return false;
    Streamable* object = node;
    int format = object->getVersion();
    if (format < 512)
        return false;
    iStream in(false, 1200);
    bool ok;
    if (Clipboard::getData((UINT)format, in))
        ok = static_cast<Streamable*>(node)->readClip(in);
    else
        ok = false;
    return ok;
}

bool TreeList::editSelected()
{
    LRESULT item = SendMessageW(TreeList_treeHwnd(this), TVM_GETNEXTITEM, TVGN_CARET, 0);
    Node* node = getItemNode((HTREEITEM)item);
    if (node)
        return editNode(node);
    return false;
}

bool TreeList::deleteSelected()
{
    HTREEITEM item = (HTREEITEM)SendMessageW(TreeList_treeHwnd(this), TVM_GETNEXTITEM, TVGN_CARET, 0);
    HTREEITEM parent = (HTREEITEM)SendMessageW(TreeList_treeHwnd(this), TVM_GETNEXTITEM, TVGN_PARENT, (LPARAM)item);
    Node* parentNode = getItemNode(parent);
    if (!item || !parentNode || !removeChildNode(parentNode, item))
        return false;
    SendMessageW(TreeList_treeHwnd(this), TVM_DELETEITEM, 0, (LPARAM)item);
    RECT rc;
    InvalidateRect(TreeList_treeHwnd(this), m_tree.getItemRect(&rc, parent), FALSE);
    onItemsChanged();
    return true;
}

bool TreeList::updateSelected()
{
    HTREEITEM item = (HTREEITEM)SendMessageW(TreeList_treeHwnd(this), TVM_GETNEXTITEM, TVGN_CARET, 0);
    HTREEITEM parent = (HTREEITEM)SendMessageW(TreeList_treeHwnd(this), TVM_GETNEXTITEM, TVGN_PARENT, (LPARAM)item);
    Node* parentNode = getItemNode(parent);
    if (!item || !parentNode || !updateChildNode(parentNode, item))
        return false;
    RECT rc;
    InvalidateRect(TreeList_treeHwnd(this), m_tree.getItemRect(&rc, item), FALSE);
    return true;
}

bool TreeList::onBeginDrag(TVITEMW* item)
{
    if (!item->lParam)
        return true;
    DropSource* source = new DropSource;
    IDataObject* data = createDataObject();
    Node* node = 0;
    Cell* cell;
    if (data || ((cell = (Cell*)item->lParam) != 0 &&
                 (node = cell->m_node,
                  (data = (IDataObject*)(INT_PTR)node->nodeV11((int)(INT_PTR)TreeList_treeHwnd(this))) != 0)))
    {
        if (source)
        {
            SetCapture(m_hWnd);
            DWORD effect = 0;
            bool dropped = DoDragDrop(data, source, getDragEffects(), &effect) == DRAGDROP_S_DROP;
            ReleaseCapture();
            if (dropped && ((effect & DROPEFFECT_MOVE) || isPerformedDropMove(data)))
            {
                if (node)
                    node->nodeV12((int)(INT_PTR)TreeList_treeHwnd(this));
                Window* parent = Window::fromHwnd(GetParent(m_hWnd), true);
                Document* doc = dynamic_cast<Document*>(parent);
                if (doc)
                    doc->setModified(true);
            }
        }
        data->Release();
    }
    if (source)
        source->Release();
    return true;
}

void TreeList::clearSelection(bool resetCaret)
{
    int count = m_selection.m_count;
    for (int i = 0; i < count; ++i)
    {
        TVITEMW tvi;
        tvi.mask = TVIF_STATE;
        tvi.hItem = (i < 0 || i >= m_selection.m_capacity) ? 0 : m_selection.m_items[i];
        tvi.stateMask = TVIS_SELECTED;
        tvi.state = 0;
        SendMessageW(TreeList_treeHwnd(this), TVM_SETITEMW, 0, (LPARAM)&tvi);
    }
    for (int i = 0; i < m_selection.m_capacity; ++i)
        m_selection.resetItem(&m_selection.m_items[i]);
    m_selection.m_count = 0;
    if (resetCaret)
        SendMessageW(TreeList_treeHwnd(this), TVM_SELECTITEM, TVGN_CARET, 0);
}

LRESULT TreeList::selectRange(HTREEITEM from, HTREEITEM to)
{
    HTREEITEM item = from;
    clearSelection(false);
    RECT rcFrom;
    RECT rcTo;
    SetRectEmpty(&rcFrom);
    SetRectEmpty(&rcTo);
    rcFrom.left = (LONG)(INT_PTR)from;
    SendMessageW(TreeList_treeHwnd(this), TVM_GETITEMRECT, TRUE, (LPARAM)&rcFrom);
    rcTo.left = (LONG)(INT_PTR)to;
    SendMessageW(TreeList_treeHwnd(this), TVM_GETITEMRECT, TRUE, (LPARAM)&rcTo);
    bool downwards = rcFrom.top < rcTo.top;
    m_selection.add(from);
    TVITEMW tvi;
    tvi.mask = TVIF_STATE;
    tvi.hItem = from;
    tvi.stateMask = TVIS_SELECTED;
    tvi.state = TVIS_SELECTED;
    LRESULT result = SendMessageW(TreeList_treeHwnd(this), TVM_SETITEMW, 0, (LPARAM)&tvi);
    if (from)
    {
        do
        {
            if (downwards)
                result = SendMessageW(TreeList_treeHwnd(this), TVM_GETNEXTITEM, TVGN_NEXT, (LPARAM)item);
            else
                result = SendMessageW(TreeList_treeHwnd(this), TVM_GETNEXTITEM, TVGN_PREVIOUS, (LPARAM)item);
            item = (HTREEITEM)result;
            if (!result)
                break;
            m_selection.add(item);
            tvi.mask = TVIF_STATE;
            tvi.hItem = item;
            tvi.stateMask = TVIS_SELECTED;
            tvi.state = TVIS_SELECTED;
            result = SendMessageW(TreeList_treeHwnd(this), TVM_SETITEMW, 0, (LPARAM)&tvi);
        } while (item != to);
    }
    return result;
}

LRESULT TreeList::setItemSelected(HTREEITEM item, bool selected)
{
    if (selected)
    {
        int count = m_selection.m_count;
        int index = 0;
        bool found = false;
        if (count > 0)
        {
            for (HTREEITEM* p = m_selection.m_items; ; ++p)
            {
                if (*p == item)
                {
                    found = true;
                    break;
                }
                if (++index >= count)
                    break;
            }
        }
        if (!found)
            m_selection.add(item);
        if (m_selection.m_count == 1)
            SendMessageW(TreeList_treeHwnd(this), TVM_SELECTITEM, TVGN_CARET, (LPARAM)item);
    }
    else
    {
        pvRemove(m_selection, item);
    }
    TVITEMW tvi;
    tvi.state = selected ? TVIS_SELECTED : 0;
    tvi.mask = TVIF_STATE;
    tvi.hItem = item;
    tvi.stateMask = TVIS_SELECTED;
    return SendMessageW(TreeList_treeHwnd(this), TVM_SETITEMW, 0, (LPARAM)&tvi);
}
