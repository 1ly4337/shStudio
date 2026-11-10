#include "ListCtrl.h"
#include "dark_mode.h"
#include <cmath>
#include "ImageStrip.h"
#include "Node.h"
#include "Exception.h"
#include "Window.h"
#include "DropTarget.h"
#include "GDC.h"
#include "WindowDC.h"
#include "PaintDC.h"
#include "dragdrop.h"
#include "gdi_cache.h"
#include "ImageList.h"
#include "ida_compat.h"
#include <cstdarg>
#include <cwchar>
#include <cstdlib>
#include <cfloat>
#include <uxtheme.h>
#include "dark_mode.h"
#include "_imports_p09.h"
#include "_imports_g7.h"

wchar_t g_listValueBuffer[64];
NMLVDISPINFOW g_listEditDispInfo;

ListCtrl::ListCtrl()
    : m_imageMap(32, 32)
{
    m_headerBottom = 0;
    m_totalWidth = 0;
    m_smallIconSize.cy = 0;
    m_smallIconSize.cx = 0;
    m_largeIconSize.cy = 0;
    m_largeIconSize.cx = 0;
    m_tileSize.cy = 0;
    m_tileSize.cx = 0;
    SetRectEmpty(&m_editRect);
    m_editFormat = 0;
    m_editCancelled = false;
    m_editNode = 0;
    m_editColumnId = -1;
    m_editMoving = true;
    m_hHeader = 0;
    m_columnCount = 0;
    m_scrollPos = 0;
    m_dragMode = 0;
    m_highlight = false;
    m_textColor = 0;
    m_smallImages = 0;
    m_largeImages = 0;
    m_trackingMouse = false;
    m_dropTarget = 0;
    m_sortColumn = 0;
    m_prevSortColumn = 0;
    m_view = 1;
    m_hotItem = -1;
    m_tileLines = 1;
    m_sortAscending = true;
}

ListCtrl::~ListCtrl()
{
    if (m_smallImages)
        ImageList_Destroy(m_smallImages);
    if (m_largeImages)
        ImageList_Destroy(m_largeImages);
}

int ListCtrl::setSmallImages(const ImageList* images)
{
    int result = IsWindow(m_hWnd);
    if (result)
    {
        if (m_smallImages)
            ImageList_Destroy(m_smallImages);
        m_smallImages = 0;
        m_imageMap.removeAll();
        HIMAGELIST copy = ImageList_Duplicate(images->m_hList);
        int count = images->m_ids.m_count;
        for (int i = 0; i < count; ++i)
        {
            long id;
            if (i < 0 || i >= images->m_ids.m_capacity)
                id = 0;
            else
                id = images->m_ids.m_items[i];
            m_imageMap.set(id, i);
        }
        m_smallImages = copy;
        result = (int)SendMessageW(m_hWnd, LVM_SETIMAGELIST, LVSIL_SMALL, (LPARAM)copy);
        m_smallIconSize.cx = 16;
        m_smallIconSize.cy = 16;
    }
    return result;
}

void ListCtrl::setImageList(HIMAGELIST list, bool isSmall)
{
    if (!IsWindow(m_hWnd))
        return;
    BOOL large = !isSmall;
    HIMAGELIST* slot = large ? &m_largeImages : &m_smallImages;
    if (*slot)
        ImageList_Destroy(*slot);
    *slot = list;
    SendMessageW(m_hWnd, LVM_SETIMAGELIST, isSmall, (LPARAM)list);
    if (!m_imageMap.m_count)
    {
        int count = ImageList_GetImageCount(list);
        for (int i = 0; i < count; ++i)
            m_imageMap.set(i + 1, i);
    }
    int cx = 0;
    int cy = 0;
    ImageList_GetIconSize(list, &cx, &cy);
    SIZE* size = large ? &m_largeIconSize : &m_smallIconSize;
    size->cx = cx;
    size->cy = cy;
}

void ListCtrl::setImages(const ImageStrip* iconSet)
{
    int size = iconSet->m_cx;
    HIMAGELIST list = ImageList_Create(size, size, ILC_COLOR32, iconSet->m_count, 64);
    ImageList_Add(list, iconSet->m_bitmap, 0);
    int imageCount = ImageList_GetImageCount(list);
    setImageList(list, size == 16);
    for (int i = 0; i < m_imageMap.m_count; ++i)
        m_imageMap.clearEntry(&m_imageMap.m_table[i]);
    m_imageMap.m_count = 0;
    for (int i = 0; i < imageCount; ++i)
    {
        m_imageMap.set(iconSet->idAt(i), i);
    }
    if (size == 32)
    {
        HBITMAP half = halveBitmap(iconSet->m_bitmap);
        HIMAGELIST smallList = ImageList_Create(16, 16, ILC_COLOR32, iconSet->m_count, 64);
        ImageList_Add(smallList, half, 0);
        DeleteObject(half);
        ImageList_GetImageCount(smallList);
        setImageList(smallList, true);
    }
}

bool ListCtrl::addColumn(const wchar_t* title, int width, int format, int columnId)
{
    if (!IsWindow(m_hWnd))
        return false;
    LVCOLUMNW column;
    memset(&column, 0, sizeof(column));
    column.fmt = format & 3;
    column.cx = width;
    column.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    column.pszText = (LPWSTR)title;
    column.iSubItem = m_columnCount;
    if (SendMessageW(m_hWnd, LVM_INSERTCOLUMNW, m_columnCount, (LPARAM)&column) == -1)
        return false;
    int id = columnId;
    if (columnId == -1)
        id = m_columnCount;
    m_columnIds.add(id);
    m_columnFormats.add((unsigned short)format);
    m_totalWidth += width;
    ++m_columnCount;
    m_columnOrder.m_count = 0;
    RECT rc;
    getWindowRect(&rc);
    int cx = rc.right - rc.left;
    int cy = rc.bottom - rc.top;
    SetWindowPos(m_hWnd, 0, 0, 0, cx - 1, cy, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    SetWindowPos(m_hWnd, 0, 0, 0, cx, cy, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    return true;
}

bool ListCtrl::deleteColumn(int column)
{
    if (!IsWindow(m_hWnd) || column < 1 || column >= m_columnCount
        || SendMessageW(m_hWnd, LVM_DELETECOLUMN, column, 0) != 1)
        return false;
    m_columnIds.removeAt(column);
    m_columnFormats.removeAt(column);
    for (int i = 0; i < m_tileColumns.m_count; ++i)
    {
        if (m_tileColumns.m_items[i] == (unsigned int)column)
        {
            m_tileColumns.remove(column);
            break;
        }
    }
    --m_columnCount;
    m_columnOrder.m_count = 0;
    refreshLayout();
    return true;
}

BOOL ListCtrl::refreshLayout()
{
    RECT rc;
    getWindowRect(&rc);
    int cx = rc.right - rc.left;
    int cy = rc.bottom - rc.top;
    SetWindowPos(m_hWnd, 0, 0, 0, cx - 1, cy, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    return SetWindowPos(m_hWnd, 0, 0, 0, cx, cy, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
}

int ListCtrl::columnFromPoint(const POINT* screenPt, LONG* range, int )
{
    if (range)
    {
        range[0] = 0;
        range[1] = 0;
    }
    if (loadColumnOrder(false))
        return -1;
    POINT pt;
    pt.x = screenPt->x;
    pt.y = screenPt->y;
    ScreenToClient(m_hWnd, &pt);
    LONG right = 0;
    int i = 0;
    int count = m_columnOrder.m_count;
    if (count <= 0)
        return -1;
    LONG left;
    for (;;)
    {
        left = right;
        WPARAM column;
        if (i < 0 || i >= m_columnOrder.m_count)
            column = 0;
        else
            column = m_columnOrder.m_items[i];
        right += (LONG)SendMessageW(m_hWnd, LVM_GETCOLUMNWIDTH, column, 0);
        if (left <= pt.x && right >= pt.x)
            break;
        if (++i >= count)
            return -1;
    }
    if (range)
    {
        range[0] = left;
        range[1] = right;
    }
    if (i < 0 || i >= m_columnOrder.m_count)
        return 0;
    return m_columnOrder.m_items[i];
}

HRESULT ListCtrl::loadColumnOrder(bool force)
{
    if (force)
        m_columnOrder.m_count = 0;
    if (m_columnOrder.m_count > 0)
        return 0;
    bool noHeader = m_hHeader == 0;
    m_headerHeight = 0;
    if (noHeader)
    {
        HWND header = (HWND)SendMessageW(m_hWnd, LVM_GETHEADER, 0, 0);
        m_hHeader = header;
        if (!header)
            return E_FAIL;
        RECT rc;
        memset(&rc, 0, sizeof(rc));
        GetClientRect(header, &rc);
        m_headerHeight = rc.bottom;
    }
    int count = (int)SendMessageW(m_hHeader, HDM_GETITEMCOUNT, 0, 0);
    if (count <= 0)
        return 4306;
    m_columnOrder.m_count = 0;
    int* order = (int*)::operator new[](((unsigned __int64)(unsigned)count >> 30) != 0 ? (size_t)-1 : 4 * count);
    bool ok = SendMessageW(m_hWnd, LVM_GETCOLUMNORDERARRAY, count, (LPARAM)order) != 0;
    if (ok)
    {
        for (int i = 0; i < count; ++i)
            m_columnOrder.add(order[i]);
    }
    ::operator delete[](order);
    HRESULT hr = E_FAIL;
    if (ok)
        return 0;
    return hr;
}

bool ListCtrl::setView(int view)
{
    if (view > 4)
        return false;
    HWND hWnd = m_hWnd;
    m_view = view;
    if (IsWindow(hWnd))
        SendMessageW(m_hWnd, LVM_SETVIEW, view, 0);
    return true;
}

bool ListCtrl::setTileView(int , const SIZE* tileSize, const primitiveArray<unsigned int>* columns)
{
    m_tileColumns.m_count = 0;
    m_tileColumns.addAll(columns->m_count, columns->m_items);
    m_tileLines = columns->m_count;
    m_tileSize.cx = tileSize->cx;
    m_tileSize.cy = tileSize->cy;
    if (!tileSize->cx)
    {
        int width = -5 - GetSystemMetrics(SM_CXVSCROLL);
        SIZE size;
        m_tileSize.cx = (width + getSize(&size)->cx) / 2;
    }
    int cx = m_tileSize.cx;
    LVTILEVIEWINFO info;
    DWORD mask = LVTVIM_COLUMNS;
    DWORD flags = 0;
    info.cbSize = 40;
    info.dwMask = LVTVIM_COLUMNS;
    info.dwFlags = 0;
    info.sizeTile.cx = 0;
    info.sizeTile.cy = 0;
    info.cLines = 2;
    info.rcLabelMargin.left = 0;
    info.rcLabelMargin.top = 0;
    info.rcLabelMargin.right = 0;
    info.rcLabelMargin.bottom = 0;
    if (cx)
    {
        mask = LVTVIM_COLUMNS | LVTVIM_TILESIZE;
        flags = LVTVIF_FIXEDWIDTH;
        info.dwMask = LVTVIM_COLUMNS | LVTVIM_TILESIZE;
        info.dwFlags = LVTVIF_FIXEDWIDTH;
        info.sizeTile.cx = cx;
    }
    int cy = m_tileSize.cy;
    if (cy)
    {
        info.dwMask = mask | LVTVIM_TILESIZE;
        info.dwFlags = flags | LVTVIF_FIXEDHEIGHT;
        info.sizeTile.cy = cy;
    }
    return SendMessageW(m_hWnd, LVM_SETTILEVIEWINFO, 0, (LPARAM)&info) == 1;
}

void ListCtrl::setHighlight(bool on)
{
    m_highlight = on;
    RECT rect;
    getClientRect(&rect);
    if (!m_hHeader)
    {
        HWND hWnd = m_hWnd;
        m_headerHeight = 0;
        HWND header = (HWND)SendMessageW(hWnd, LVM_GETHEADER, 0, 0);
        m_hHeader = header;
        if (header)
        {
            RECT rc;
            SetRectEmpty(&rc);
            GetClientRect(header, &rc);
            m_headerHeight = rc.bottom;
        }
    }
    rect.top = m_headerHeight - 1;
    InvalidateRect(m_hWnd, &rect, TRUE);
    rect.top += 2;
    rect.bottom -= 2;
    rect.left += 2;
    rect.right -= 2;
    ValidateRect(m_hWnd, &rect);
    UpdateWindow(m_hWnd);
}

int ListCtrl::insertItem(Node* node, int index)
{
    if (!IsWindow(m_hWnd) || !node)
        return -1;
    LVITEMW item;
    memset(&item, 0, sizeof(item));
    item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
    if (index >= 0)
        item.iItem = index;
    else
        item.iItem = getCount(0);
    item.pszText = LPSTR_TEXTCALLBACKW;
    item.iImage = I_IMAGECALLBACK;
    item.lParam = (LPARAM)node;
    if (m_tileColumns.m_count >= m_tileLines)
    {
        item.mask |= LVIF_COLUMNS;
        item.cColumns = m_tileLines;
        if (m_tileColumns.m_count <= 1)
            throw ArrayIndexOutOfBoundsException(1, m_tileColumns.m_count);
        item.puColumns = m_tileColumns.m_items + 1;
    }
    int result = (int)SendMessageW(m_hWnd, LVM_INSERTITEMW, 0, (LPARAM)&item);
    if (result < 0)
        return -1;
    item.mask = LVIF_TEXT;
    for (int subItem = 1; subItem < m_columnCount; ++subItem)
    {
        item.iSubItem = subItem;
        item.pszText = LPSTR_TEXTCALLBACKW;
        SendMessageW(m_hWnd, LVM_SETITEMTEXTW, result, (LPARAM)&item);
    }
    return result;
}

int ListCtrl::insertTextItem(const wchar_t* text, int image, int index)
{
    LVITEMW item;
    memset(&item, 0, sizeof(item));
    UINT mask = LVIF_TEXT;
    item.mask = LVIF_TEXT;
    if (index < 0)
    {
        index = getCount(0);
        mask = item.mask;
    }
    item.iItem = index;
    item.pszText = (LPWSTR)text;
    if (m_tileColumns.m_count >= m_tileLines)
    {
        item.cColumns = m_tileLines;
        mask |= LVIF_COLUMNS;
        item.mask = mask;
        if (m_tileColumns.m_count <= 1)
            throw ArrayIndexOutOfBoundsException(1, m_tileColumns.m_count);
        item.puColumns = m_tileColumns.m_items + 1;
    }
    if (image >= 0)
    {
        item.mask = mask | LVIF_IMAGE;
        item.iImage = image;
    }
    int result = (int)SendMessageW(m_hWnd, LVM_INSERTITEMW, 0, (LPARAM)&item);
    if (result >= 0 && text == LPSTR_TEXTCALLBACKW)
    {
        item.iItem = result;
        item.mask = LVIF_TEXT;
        for (int subItem = 1; subItem < m_columnCount; ++subItem)
        {
            item.iSubItem = subItem;
            SendMessageW(m_hWnd, LVM_SETITEMTEXTW, result, (LPARAM)&item);
        }
    }
    return result;
}

int ListCtrl::insertRow(ListCtrl* list, const wchar_t* text, ...)
{
    int result = list->insertTextItem(text, -1, -1);
    va_list args;
    va_start(args, text);
    const wchar_t* subText = va_arg(args, const wchar_t*);
    int subItem = 1;
    if (subText)
    {
        do
        {
            LVITEMW item;
            memset(&item.iItem, 0, sizeof(item) - sizeof(item.mask));
            item.iSubItem = subItem;
            item.mask = LVIF_TEXT;
            item.iItem = result;
            item.pszText = (LPWSTR)subText;
            SendMessageW(list->m_hWnd, LVM_SETITEMTEXTW, result, (LPARAM)&item);
            subText = va_arg(args, const wchar_t*);
            ++subItem;
        } while (subText);
    }
    va_end(args);
    return result;
}

bool ListCtrl::setCellText(int index, int subItem, const wchar_t* text)
{
    LVITEMW item;
    memset(&item.iItem, 0, sizeof(item) - sizeof(item.mask));
    item.iSubItem = subItem;
    item.pszText = (LPWSTR)text;
    item.mask = LVIF_TEXT;
    item.iItem = index;
    return SendMessageW(m_hWnd, LVM_SETITEMTEXTW, index, (LPARAM)&item) == 1;
}

bool ListCtrl::insertNumberRow(const wchar_t* text, int value, int )
{
    wchar_t buffer[16];
    memset(buffer, 0, sizeof(buffer));
    _ltow_s(value, buffer, 16, 10);
    int index = insertTextItem(text, 0, -1);
    if (index < 0)
        return false;
    LVITEMW item;
    memset(&item.iItem, 0, sizeof(item) - sizeof(item.mask));
    item.pszText = buffer;
    item.mask = LVIF_TEXT;
    item.iItem = index;
    item.iSubItem = 1;
    return SendMessageW(m_hWnd, LVM_SETITEMTEXTW, index, (LPARAM)&item) == 1;
}

RECT* ListCtrl::getItemRect(RECT* rc, int index, int code)
{
    SetRect(rc, code, 0, 0, 0);
    if (!SendMessageW(m_hWnd, LVM_GETITEMRECT, index, (LPARAM)rc))
        SetRectEmpty(rc);
    return rc;
}

int ListCtrl::getItemImage(int index)
{
    if (IsWindow(m_hWnd))
        return -1;
    NMLVDISPINFOW info;
    memset(&info.hdr.idFrom, 0, sizeof(info) - sizeof(info.hdr.hwndFrom));
    info.hdr.hwndFrom = m_hWnd;
    info.hdr.idFrom = GetDlgCtrlID(info.hdr.hwndFrom);
    info.item.iItem = index;
    info.hdr.code = LVN_GETDISPINFOW;
    info.item.mask = LVIF_IMAGE;
    info.item.iImage = -1;
    if (!SendMessageW(m_hWnd, LVM_GETITEMW, 0, (LPARAM)&info.item))
        return -1;
    int result = info.item.iImage;
    if (info.item.iImage == -1)
    {
        HWND parent = GetParent(m_hWnd);
        SendMessageW(parent, WM_NOTIFY, 0, (LPARAM)&info);
        return info.item.iImage;
    }
    return result;
}

String ListCtrl::getCellText(int index, int subItem)
{
    String text;
    if (IsWindow(m_hWnd))
    {
        wchar_t buffer[256];
        memset(buffer, 0, sizeof(buffer));
        NMLVDISPINFOW info;
        memset(&info.hdr.idFrom, 0, sizeof(info) - sizeof(info.hdr.hwndFrom));
        info.hdr.hwndFrom = m_hWnd;
        info.hdr.idFrom = GetDlgCtrlID(info.hdr.hwndFrom);
        info.item.iItem = index;
        info.item.iSubItem = subItem;
        info.item.pszText = buffer;
        info.hdr.code = LVN_GETDISPINFOW;
        info.item.mask = LVIF_TEXT;
        info.item.cchTextMax = 256;
        if (SendMessageW(m_hWnd, LVM_GETITEMW, 0, (LPARAM)&info.item))
        {
            const wchar_t* value = info.item.pszText;
            if (info.item.pszText == LPSTR_TEXTCALLBACKW)
            {
                HWND parent = GetParent(m_hWnd);
                SendMessageW(parent, WM_NOTIFY, 0, (LPARAM)&info);
                value = info.item.pszText;
            }
            if (value)
            {
                unsigned length = (unsigned)wcslen(value);
                if (length)
                    text.append(value, length);
            }
        }
    }
    return text;
}

const wchar_t* ListCtrl::formatValue(Node* node, int columnId, short format)
{
    memset(g_listValueBuffer, 0, sizeof(g_listValueBuffer));
    if ((format & 0xF000) == LCF_OWNERTEXT)
    {
        const wchar_t* text = getItemText(node, columnId);
        if (!text)
            return L"";
        if (wcslen(text) > 0x3F)
            return text;
        swprintf_s(g_listValueBuffer, 64, L"%s", text);
        return g_listValueBuffer;
    }
    short kind = format & LCF_VALUE_MASK;
    if (!kind)
        return node->getString(columnId);
    if (kind == LCF_NONE)
        return 0;
    int raw = node->getInt(columnId);
    switch (kind)
    {
    case LCF_SIGNED:
        swprintf_s(g_listValueBuffer, 64, L"%li", raw);
        return g_listValueBuffer;
    case LCF_UNSIGNED:
        swprintf_s(g_listValueBuffer, 64, L"%lu", raw);
        return g_listValueBuffer;
    case LCF_HEX:
        swprintf_s(g_listValueBuffer, 64, L"0x%lX", raw);
        return g_listValueBuffer;
    }
    if (kind != LCF_FLOAT)
        return 0;
    float value;
    memcpy(&value, &raw, sizeof(value));
    _fpclass(value);
    float magnitude = fabsf(value);
    if (magnitude < 0.00001)
        return L"0.0";
    if (magnitude >= 1.0)
    {
        if (magnitude >= 10.0)
        {
            double d = value;
            if (magnitude >= 100.0)
                swprintf_s(g_listValueBuffer, 64, L"%.2f", d);
            else
                swprintf_s(g_listValueBuffer, 64, L"%.3f", d);
        }
        else
        {
            swprintf_s(g_listValueBuffer, 64, L"%.4f", (double)value);
        }
    }
    else
    {
        swprintf_s(g_listValueBuffer, 64, L"%.5f", (double)value);
    }
    return g_listValueBuffer;
}

Node* ListCtrl::getItemParam(int index)
{
    if (!IsWindow(m_hWnd) || index < 0)
        return 0;
    LVITEMW item;
    memset(&item.iItem, 0, sizeof(item) - sizeof(item.mask));
    item.mask = LVIF_PARAM;
    item.iItem = index;
    LRESULT ok = SendMessageW(m_hWnd, LVM_GETITEMW, 0, (LPARAM)&item);
    return ok != 0 ? (Node*)item.lParam : 0;
}

Node* ListCtrl::getNode(int index)
{
    return getItemParam(index);
}

LRESULT ListCtrl::setItemState(int index, UINT state, UINT mask)
{
    LVITEMW item;
    memset(&item, 0, sizeof(item));
    item.stateMask = mask | state;
    item.state = state;
    return SendMessageW(m_hWnd, LVM_SETITEMSTATE, index, (LPARAM)&item);
}

Node* ListCtrl::nodeFromPoint(POINT pt)
{
    int count = getCount(0);
    int i = 0;
    if (count <= 0)
        return 0;
    for (;;)
    {
        RECT rc;
        getItemRect(&rc, i, 0);
        if (PtInRect(&rc, pt))
            break;
        if (++i >= count)
            return 0;
    }
    return getNode(i);
}

int ListCtrl::getNextSelected(int start)
{
    if (!IsWindow(m_hWnd) || getCount(1) < 1)
        return -1;
    int i = start;
    int count = getCount(0);
    if (start >= count)
        return -1;
    while (SendMessageW(m_hWnd, LVM_GETITEMSTATE, i, LVIS_SELECTED) != LVIS_SELECTED)
    {
        if (++i >= count)
            return -1;
    }
    return i;
}

void ListCtrl::focusItem(int index, bool redraw)
{
    if (IsWindow(m_hWnd))
    {
        LVITEMW item;
        memset(&item, 0, sizeof(item));
        item.stateMask = LVIS_FOCUSED;
        item.state = LVIS_FOCUSED;
        SendMessageW(m_hWnd, LVM_SETITEMSTATE, index, (LPARAM)&item);
        if (redraw)
        {
            RECT rc;
            InvalidateRect(m_hWnd, getItemRect(&rc, index, 0), TRUE);
        }
    }
}

void ListCtrl::selectItem(int index)
{
    if (!IsWindow(m_hWnd))
        return;
    int count = getCount(0);
    int selected = getCount(1);
    if (selected > 0)
    {
        for (int i = 0; i < count; ++i)
        {
            if (SendMessageW(m_hWnd, LVM_GETITEMSTATE, i, LVIS_SELECTED) == LVIS_SELECTED)
            {
                if (i != index)
                {
                    LVITEMW item;
                    memset(&item, 0, sizeof(item));
                    item.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
                    item.state = 0;
                    SendMessageW(m_hWnd, LVM_SETITEMSTATE, i, (LPARAM)&item);
                    updateItem(i);
                }
                if (!--selected)
                    break;
            }
        }
    }
    if (index >= 0 && index < count)
    {
        LVITEMW item;
        memset(&item, 0, sizeof(item));
        item.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
        item.state = LVIS_FOCUSED | LVIS_SELECTED;
        SendMessageW(m_hWnd, LVM_SETITEMSTATE, index, (LPARAM)&item);
        SendMessageW(m_hWnd, LVM_ENSUREVISIBLE, index, 0);
    }
}

void ListCtrl::updateItem(int index)
{
    if (index == -2)
        index = getNextSelected(0);
    if (index >= 0)
    {
        if (getItemParam(index))
        {
            LVITEMW item;
            memset(&item, 0, sizeof(item));
            for (int i = 0; i < m_columnCount; ++i)
            {
                item.iSubItem = i;
                item.pszText = LPSTR_TEXTCALLBACKW;
                SendMessageW(m_hWnd, LVM_SETITEMTEXTW, index, (LPARAM)&item);
            }
        }
        RECT rc;
        InvalidateRect(m_hWnd, getItemRect(&rc, index, 0), TRUE);
    }
}

void ListCtrl::updateAll()
{
    if (!m_hWnd)
        return;
    setRedraw(false);
    int count = getCount(0);
    if (count > 0)
    {
        for (int index = 0; index < count; ++index)
        {
            if (getItemParam(index))
            {
                LVITEMW item;
                memset(&item, 0, sizeof(item));
                for (int i = 0; i < m_columnCount; ++i)
                {
                    item.iSubItem = i;
                    item.pszText = LPSTR_TEXTCALLBACKW;
                    SendMessageW(m_hWnd, LVM_SETITEMTEXTW, index, (LPARAM)&item);
                }
            }
        }
        if (count > 0)
            SendMessageW(m_hWnd, LVM_REDRAWITEMS, 0, count - 1);
    }
    setRedraw(true);
    UpdateWindow(m_hWnd);
}

bool ListCtrl::onCreate()
{
    SendMessageW(m_hWnd, LVM_SETVIEW, m_view, 0);
    DWORD exStyle = getExStyle();
    if (IsWindow(m_hWnd))
    {
        DWORD current = (DWORD)SendMessageW(m_hWnd, LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0);
        SendMessageW(m_hWnd, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, exStyle | (current & ~(~exStyle & LVS_EX_TRACKSELECT)));
    }
    return true;
}

bool ListCtrl::onCommand(int code, LPARAM ctrl, int id)
{
    if ((code | 1) != 1 || ctrl)
        return false;
    if (id == 0xE12A)
    {
        insertNew(-1);
        return true;
    }
    if (id != 0xE12B)
        return false;
    deleteSelected(L"to delete");
    return true;
}

void ListCtrl::onFocus(bool gained)
{
    if (!gained)
        return;
    SetFocus(m_hWnd);
    int selected = getCount(1);
    if (!selected)
        return;
    int i = 0;
    int count = getCount(0);
    if (count <= 0)
        return;
    do
    {
        if (SendMessageW(m_hWnd, LVM_GETITEMSTATE, i, LVIS_SELECTED) == LVIS_SELECTED)
        {
            if (SendMessageW(m_hWnd, LVM_ISITEMVISIBLE, i, 0) == 1)
            {
                RECT rc;
                InvalidateRect(m_hWnd, getItemRect(&rc, i, 0), TRUE);
            }
            --selected;
        }
        if (!selected)
            break;
        ++i;
    } while (i < count);
}

bool ListCtrl::onKey(short vk, int flags, bool down)
{
    if (!down)
        return false;
    switch (vk)
    {
    case VK_RETURN:
    {
        int index = getNextSelected(0);
        if (index >= 0)
        {
            activateItem(index, flags);
            selectItem(index);
            return true;
        }
        return true;
    }
    case VK_ESCAPE:
        if (g_dragImage)
        {
            endDragImage();
            return true;
        }
        return true;
    case VK_DELETE:
        deleteSelected(L"to delete");
        return true;
    case VK_F2:
    {
        int index = getNextSelected(0);
        if (index >= 0)
        {
            SetFocus(m_hWnd);
            HWND edit = (HWND)SendMessageW(m_hWnd, LVM_EDITLABELW, index, 0);
            if (edit)
                GetWindowLongW(edit, GWL_STYLE);
        }
        return true;
    }
    }
    return Control::onKey(vk, flags, down);
}

void ListCtrl::onSize(int , int , int )
{
    SCROLLINFO info;
    memset(&info.nMin, 0, 20);
    info.cbSize = sizeof(SCROLLINFO);
    info.fMask = SIF_RANGE | SIF_POS;
    GetScrollInfo(m_hWnd, SB_HORZ, &info);
    m_scrollPos = info.nPos;
}

bool ListCtrl::onLButton(int , POINT* pt, bool down)
{
    if (!g_dragImage)
    {
        m_editCancelled = true;
        return false;
    }
    if (down)
        return false;
    DropTarget* target = (DropTarget*)m_dropTarget;
    if (!target)
    {
        int dragItem = g_dragImage->m_param34;
        LVHITTESTINFO hit;
        hit.pt = *pt;
        hit.flags = 0;
        hit.iItem = -1;
        hit.iSubItem = 0;
        hit.iGroup = 0;
        SendMessageW(m_hWnd, LVM_HITTEST, 0, (LPARAM)&hit);
        if ((hit.flags & LVHT_ONITEM) == 0 || hit.iItem == -1)
        {
            ClientToScreen(m_hWnd, pt);
            onDropOutside(hit.flags, pt, dragItem);
        }
        else if (dragItem != hit.iItem && canMoveItem(dragItem, hit.iItem))
        {
            int count = getCount(0);
            int high = hit.iItem;
            int low = hit.iItem;
            if (dragItem > hit.iItem)
                high = dragItem;
            if (dragItem < hit.iItem)
                low = dragItem;
            int last = count - 1;
            int to = high + 1;
            if (last < to)
                to = last;
            int from = low - 1;
            if (from < 0)
                from = 0;
            SendMessageW(m_hWnd, LVM_REDRAWITEMS, from, to);
            setRedraw(false);
            int view = m_view;
            HWND hWnd = m_hWnd;
            bool restore = true;
            if (view)
            {
                m_view = 0;
                if (IsWindow(hWnd))
                    SendMessageW(m_hWnd, LVM_SETVIEW, 0, 0);
                if (view < 0)
                    restore = false;
            }
            else
            {
                m_view = 3;
                if (IsWindow(hWnd))
                    SendMessageW(m_hWnd, LVM_SETVIEW, 3, 0);
            }
            if (restore)
                setView(view);
            setRedraw(true);
            endDragImage();
            return true;
        }
        endDragImage();
        return true;
    }
    target->setDropHighlight(false);
    int format;
    HGLOBAL data;
    if (g_dragImage)
    {
        data = g_dragImage->m_hData;
        format = g_dragImage->m_param38;
    }
    else
    {
        data = 0;
        format = 0;
    }
    ((DropTarget*)m_dropTarget)->dropGlobal(format, data);
    m_dropTarget = 0;
    endDragImage();
    return true;
}

bool ListCtrl::onRButton(int , POINT* pt, bool down)
{
    if (!down)
        return false;
    if (!m_hHeader || !m_headerBottom)
    {
        HWND header = (HWND)SendMessageW(m_hWnd, LVM_GETHEADER, 0, 0);
        m_hHeader = header;
        if (header)
        {
            RECT rc;
            SetRectEmpty(&rc);
            GetClientRect(header, &rc);
            m_headerBottom = rc.bottom;
        }
    }
    UINT msg = 0x40C;
    if (pt->y <= m_headerBottom)
        msg = 0x407;
    HWND owner = m_hWnd;
    LRESULT menuId = SendMessageW(owner, msg, m_id, (LPARAM)owner);
    if (menuId <= 0)
    {
        if (msg == 0x407)
        {
            owner = GetParent(m_hWnd);
            menuId = SendMessageW(owner, 0x407, m_id, (LPARAM)m_hWnd);
        }
        if (menuId <= 0)
            return true;
    }
    HMENU menu = LoadMenuW(0, MAKEINTRESOURCEW((unsigned short)menuId));
    if (menu)
    {
        HMENU popup = GetSubMenu(menu, 0);
        if (popup)
        {
            SetMenuDefaultItem(popup, (UINT)-1, TRUE);
            SendMessageW(owner, 0x408, (WPARAM)popup, (LPARAM)m_hWnd);
            ClientToScreen(m_hWnd, pt);
            TrackPopupMenu(popup, 0, pt->x, pt->y, 0, owner, 0);
        }
        DestroyMenu(menu);
    }
    return true;
}

bool ListCtrl::onMouseMove(int , POINT* pt)
{
    if (g_dragImage)
    {
        moveDragImage(pt);
        RECT rc;
        GetClientRect(m_hWnd, &rc);
        if (!PtInRect(&rc, *pt))
        {
            ClientToScreen(m_hWnd, pt);
            HWND under = Window_fromPoint(pt);
            if (under)
            {
                DropTarget* target = dynamic_cast<DropTarget*>(Window::fromHwnd(under, false));
                if (target || (target = dynamic_cast<DropTarget*>(Control::fromHandle(under))) != 0)
                {
                    DropTarget* current = (DropTarget*)m_dropTarget;
                    if (target != current)
                    {
                        if (current)
                            current->setDropHighlight(false);
                        DropTarget* source = g_dragImage ? (DropTarget*)g_dragImage->m_param30 : 0;
                        if (target != source)
                        {
                            int format = g_dragImage ? g_dragImage->m_param38 : 0;
                            if (target->acceptsFormat(format))
                            {
                                m_dropTarget = target;
                                target->setDropHighlight(true);
                            }
                        }
                    }
                    return true;
                }
            }
            DropTarget* current = (DropTarget*)m_dropTarget;
            if (current)
            {
                current->setDropHighlight(false);
                m_dropTarget = 0;
            }
        }
        return true;
    }
    if (m_view != 1)
    {
        if (!m_trackingMouse)
        {
            TRACKMOUSEEVENT track;
            track.hwndTrack = m_hWnd;
            track.cbSize = 16;
            track.dwFlags = TME_LEAVE;
            track.dwHoverTime = HOVER_DEFAULT;
            m_trackingMouse = TrackMouseEvent(&track) != 0;
        }
        LVHITTESTINFO hit;
        hit.pt = *pt;
        hit.flags = 0;
        hit.iItem = -1;
        hit.iSubItem = 0;
        hit.iGroup = 0;
        if (SendMessageW(m_hWnd, LVM_HITTEST, 0, (LPARAM)&hit) == -1)
        {
            if (m_hotItem != -1)
            {
                bool selected = SendMessageW(m_hWnd, LVM_GETITEMSTATE, m_hotItem, LVIS_SELECTED) == LVIS_SELECTED;
                RECT rc;
                InvalidateRect(m_hWnd, getItemRect(&rc, m_hotItem, 0), !selected);
            }
            m_hotItem = -1;
        }
        else if (m_hotItem != hit.iItem)
        {
            RECT rc;
            if (m_hotItem != -1)
                InvalidateRect(m_hWnd, getItemRect(&rc, m_hotItem, 0), TRUE);
            m_hotItem = hit.iItem;
            InvalidateRect(m_hWnd, getItemRect(&rc, hit.iItem, 0), FALSE);
        }
    }
    return false;
}

bool ListCtrl::onNotify(NMHDR* hdr, LRESULT* result)
{
    switch ((int)hdr->code)
    {
    case NM_RCLICK:
    {
        POINT pt;
        GetCursorPos(&pt);
        ScreenToClient(m_hWnd, &pt);
        if (hdr->hwndFrom == (HWND)SendMessageW(m_hWnd, LVM_GETHEADER, 0, 0))
        {
            POINT clickPt;
            clickPt.x = pt.x;
            clickPt.y = pt.y;
            onRButton(0, &clickPt, true);
            return true;
        }
        nodeFromPoint(pt);
        return true;
    }
    case HDN_ENDDRAG:
        *result = 0;
        m_columnOrder.m_count = 0;
        invalidate(true);
        return true;
    case NM_CUSTOMDRAW:
        *result = onCustomDraw((NMLVCUSTOMDRAW*)hdr, (int)hdr->idFrom);
        return true;
    case LVN_ITEMCHANGED:
    {
        NMLISTVIEW* nm = (NMLISTVIEW*)hdr;
        if (nm->iItem == -1 && nm->uOldState == LVIS_SELECTED)
        {
            int index = getNextSelected(0);
            nm->iItem = index;
            if (index >= 0)
                updateItem(index);
        }
        else if (((nm->uNewState ^ nm->uOldState) & (LVIS_FOCUSED | LVIS_SELECTED)) != 0)
        {
            updateItem(nm->iItem);
            UpdateWindow(m_hWnd);
        }
        onItemChanged(nm->iItem, nm->uNewState);
        return true;
    }
    case LVN_BEGINLABELEDITW:
        *result = onBeginLabelEdit((NMLVDISPINFOW*)hdr);
        return true;
    case LVN_ENDLABELEDITW:
        onEndLabelEdit((NMLVDISPINFOW*)hdr);
        return true;
    case LVN_COLUMNCLICK:
        onColumnClick((NMLISTVIEW*)hdr);
        return true;
    case LVN_BEGINDRAG:
        onBeginDrag((NMLISTVIEW*)hdr);
        return true;
    case LVN_ITEMACTIVATE:
    {
        NMITEMACTIVATE* nm = (NMITEMACTIVATE*)hdr;
        activateItem(nm->iItem, nm->uKeyFlags);
        selectItem(nm->iItem);
        return true;
    }
    case LVN_ODSTATECHANGED:
    {
        NMLVODSTATECHANGE* nm = (NMLVODSTATECHANGE*)hdr;
        for (int i = nm->iFrom; i <= nm->iTo; ++i)
        {
            RECT rc;
            InvalidateRect(m_hWnd, getItemRect(&rc, i, 0), TRUE);
        }
        return true;
    }
    case LVN_GETDISPINFOW:
        return onGetDispInfo((NMLVDISPINFOW*)hdr) == 0;
    case LVN_ENDSCROLL:
    {
        SCROLLINFO info;
        memset(&info.nMin, 0, 20);
        info.cbSize = sizeof(SCROLLINFO);
        info.fMask = SIF_RANGE | SIF_POS;
        GetScrollInfo(m_hWnd, SB_HORZ, &info);
        if (m_scrollPos != info.nPos)
        {
            m_scrollPos = info.nPos;
            invalidate(true);
            UpdateWindow(m_hWnd);
            return true;
        }
        return true;
    }
    default:
        return false;
    }
}

void ListCtrl::onColumnClick(NMLISTVIEW* nm)
{
    int subItem = nm->iSubItem;
    if (subItem < 0 || subItem >= m_columnIds.m_count)
        throw ArrayIndexOutOfBoundsException(subItem, m_columnIds.m_count);
    long columnId = m_columnIds.m_items[subItem];
    if (columnId == m_sortColumn)
    {
        m_sortAscending = !m_sortAscending;
    }
    else
    {
        m_prevSortColumn = m_sortColumn;
        m_sortColumn = columnId;
    }
    if (sort())
        updateAll();
}

int CALLBACK ListCtrl::compareProc(LPARAM a, LPARAM b, LPARAM list)
{
    if (list)
        return ((ListCtrl*)list)->compareItems((Node*)a, (Node*)b);
    return 0;
}

bool ListCtrl::sort()
{
    return SendMessageW(m_hWnd, LVM_SORTITEMS, (WPARAM)this, (LPARAM)compareProc) == 1;
}

void ListCtrl::onBeginDrag(NMLISTVIEW* nm)
{
    if (!m_dragMode)
        return;
    int selected = getCount(1);
    int remaining = selected;
    if (!selected)
        return;
    if (selected > 1 && m_dragMode != 2)
        return;
    POINT cursor = nm->ptAction;
    LVHITTESTINFO hit;
    hit.pt = nm->ptAction;
    hit.flags = 0;
    hit.iItem = 0;
    hit.iSubItem = 0;
    hit.iGroup = 0;
    int hitItem = (int)SendMessageW(m_hWnd, LVM_HITTEST, 0, (LPARAM)&hit);
    Node* hitNode = getItemParam(hitItem);
    if (!hitNode)
        return;
    int format = static_cast<Streamable*>(hitNode)->getVersion();
    ScreenToClient(m_hWnd, &cursor);
    oStream out(false, 1200);
    out << (unsigned long)selected;
    primitiveArray<long> dragged;
    RECT bounds;
    SetRectEmpty(&bounds);
    int imaged = 0;
    int count = getCount(0);
    for (int i = 0; i < count; ++i)
    {
        if (SendMessageW(m_hWnd, LVM_GETITEMSTATE, i, LVIS_SELECTED) != LVIS_SELECTED)
            continue;
        Node* node = getItemParam(i);
        if (!node)
            return;
        static_cast<Streamable*>(node)->write(out, 'clip');
        if (imaged < 5 && abs(i - hitItem) < 5)
        {
            RECT rc;
            SetRect(&rc, 0, 0, 0, 0);
            if (!SendMessageW(m_hWnd, LVM_GETITEMRECT, i, (LPARAM)&rc))
                return;
            UnionRect(&bounds, &bounds, &rc);
            ++imaged;
        }
        setItemState(i, 0, LVIS_SELECTED);
        RECT itemRc;
        InvalidateRect(m_hWnd, getItemRect(&itemRc, i, 0), TRUE);
        dragged.add(i);
        if (!--remaining)
            break;
    }
    UpdateWindow(m_hWnd);
    int dx = -bounds.left;
    bounds.right -= bounds.left;
    bounds.bottom -= bounds.top;
    int dy = -bounds.top;
    bounds.left = 0;
    bounds.top = 0;
    LONG width = bounds.right;
    LONG height = bounds.bottom;
    HDC screen = GetDC(0);
    HDC memory = CreateCompatibleDC(screen);
    HBITMAP bitmap = CreateCompatibleBitmap(screen, width, height);
    HGDIOBJ oldBitmap = SelectObject(memory, bitmap);
    HFONT font = (HFONT)SendMessageW(m_hWnd, WM_GETFONT, 0, 0);
    {
        GDC dc(memory);
        dc.m_saved0C = SelectObject(memory, font);
        HBRUSH white = CreateSolidBrush(0xFFFFFF);
        UnrealizeObject(white);
        FillRect(memory, &bounds, white);
        DeleteObject(white);
        int k = 0;
        bool complete = true;
        if (dragged.m_count > 0)
        {
            do
            {
                if (k < 0 || k >= dragged.m_count)
                    throw ArrayIndexOutOfBoundsException(k, dragged.m_count);
                int index = dragged.m_items[k];
                RECT rc;
                SetRect(&rc, 0, 0, 0, 0);
                if (!SendMessageW(m_hWnd, LVM_GETITEMRECT, index, (LPARAM)&rc))
                {
                    complete = false;
                    break;
                }
                rc.left += dx;
                rc.right += dx;
                rc.top += dy;
                rc.bottom += dy;
                Node* node = getItemParam(index);
                drawItem(memory, index, (POINT*)&rc, 0x2000, node);
            } while (++k < dragged.m_count);
        }
        if (complete)
        {
            SelectObject(memory, oldBitmap);
            DeleteDC(memory);
            ReleaseDC(0, screen);
            POINT parentOrigin;
            parentOrigin.x = 0;
            parentOrigin.y = 0;
            ClientToScreen((HWND)GetWindowLongW(m_hWnd, GWLP_HWNDPARENT), &parentOrigin);
            POINT origin;
            origin.x = -dx;
            origin.y = -dy;
            ClientToScreen(m_hWnd, &origin);
            HGLOBAL global = GlobalAlloc(GMEM_MOVEABLE, out.m_size);
            if (global)
            {
                void* dst = GlobalLock(global);
                if (dst)
                {
                    memcpy(dst, out.m_data, out.m_size);
                    GlobalUnlock(global);
                    beginDragImage((int)this, hitItem, format, global, m_hWnd, bitmap);
                    showDragImage(&origin, &cursor);
                }
                else
                {
                    GlobalFree(global);
                }
            }
        }
    }
}

LRESULT ListCtrl::onCustomDraw(NMLVCUSTOMDRAW* cd, int ctrlId)
{
    if (ctrlId != m_id)
    {
        if (darkModeOn() && cd->nmcd.hdr.hwndFrom == (HWND)SendMessageW(m_hWnd, LVM_GETHEADER, 0, 0))
        {
            if (cd->nmcd.dwDrawStage == CDDS_PREPAINT)
                return CDRF_NOTIFYITEMDRAW;
            if (cd->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
            {
                SetTextColor(cd->nmcd.hdc, dmFg(RGB(0, 0, 0)));
                return CDRF_DODEFAULT;
            }
        }
        return 0;
    }
    DWORD stage = cd->nmcd.dwDrawStage;
    if (stage == CDDS_PREPAINT)
    {
        if (!IsRectEmpty(&cd->nmcd.rc))
        {
            cd->nmcd.uItemState &= ~CDIS_FOCUS;
            return CDRF_NOTIFYITEMDRAW;
        }
        return 0;
    }
    if (stage != CDDS_ITEMPREPAINT)
        return 0;
    int index = (int)cd->nmcd.dwItemSpec;
    RECT rc;
    getItemRect(&rc, index, 0);
    Node* node = getItemParam(index);
    int state = (int)SendMessageW(m_hWnd, LVM_GETITEMSTATE, index, -1);
    return drawItem(cd->nmcd.hdc, index, (POINT*)&rc, state, node);
}

int ListCtrl::drawItem(HDC dc, int index, POINT* origin, int flags, Node* node)
{
    RECT client;
    getClientRect(&client);
    RECT rc;
    getItemRect(&rc, index, 0);
    LONG left = rc.left;
    int dx = origin->x - rc.left;
    int dy = origin->y - rc.top;
    if (!flags && index != m_hotItem && (!node || m_view != 1))
        return 0;
    GDC gdc(dc);
    HGDIOBJ savedFont = 0;
    if ((flags & ~1) != 0x2000)
    {
        int color;
        if ((flags & ~1) != 0)
        {
            color = (m_hotItem == index) + 101;
        }
        else
        {
            color = 100;
            if (m_hotItem == -1)
                color = 0;
        }
        if (color == 101 && (m_flags & 0x10) == 0)
            color = 103;
        LONG right = rc.right;
        if (rc.left < 0)
            left = 0;
        rc.left = left;
        if (client.right < rc.right)
            right = client.right;
        rc.right = right;
        if (color)
        {
            if (m_view == 1)
            {
                if ((SendMessageW(m_hWnd, LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0) & LVS_EX_GRIDLINES) != 0)
                {
                    ++rc.left;
                    --rc.right;
                    --rc.bottom;
                }
                else
                {
                    rc.left += 2;
                    rc.right -= 2;
                }
            }
            HPEN pen = CreatePen(PS_SOLID, 1, getStdColor(color));
            gdc.m_savedPen = SelectObject(dc, pen);
            HBRUSH brush = getSolidBrush(getStdColor(color + 10));
            gdc.m_saved14 = SelectObject(dc, brush);
            RoundRect(dc, rc.left, rc.top, rc.right, rc.bottom, 5, 5);
        }
        else if (m_view == 1)
        {
            HBRUSH white = CreateSolidBrush(dmBk(0xFFFFFF));
            UnrealizeObject(white);
            FillRect(dc, &rc, white);
            DeleteObject(white);
        }
    }
    int image;
    if (node)
    {
        long id = node->getId();
        int i = 0;
        image = -1;
        for (; i < m_imageMap.m_count; ++i)
            if (m_imageMap.m_table[i].key == id)
                break;
        if (i < m_imageMap.m_count && i != -1)
            image = m_imageMap.m_table[i].value;
    }
    else
    {
        image = getItemImage(index);
    }
    SetBkMode(dc, TRANSPARENT);
    RECT icon;
    getItemRect(&icon, index, LVIR_ICON);
    if (image >= 0 && icon.left >= 0 && icon.right - icon.left > 0)
    {
        int l = dx + icon.left;
        int r = dx + icon.right;
        int t = dy + icon.top;
        int b = dy + icon.bottom;
        icon.left += dx;
        icon.right += dx;
        icon.top += dy;
        icon.bottom += dy;
        if (m_view == 0)
            ImageList_Draw(m_largeImages, image, dc, (icon.left + r - m_largeIconSize.cx) / 2, t + 2, 0);
        else if (m_view == 4)
            ImageList_Draw(m_largeImages, image, dc, (l + r - m_largeIconSize.cx) / 2 - 1, (b - m_largeIconSize.cy + t) / 2, 0);
        else
            ImageList_Draw(m_smallImages, image, dc, l, t, 0);
    }
    SetTextColor(dc, m_textColor);
    String text = getCellText(index, 0);
    RECT labelTmp;
    RECT label;
    CopyRect(&label, getItemRect(&labelTmp, index, LVIR_LABEL));
    label.right += dx;
    int top = dy + label.top;
    int bottom = dy + label.bottom;
    int labelLeft = dx + label.left;
    int view = m_view;
    label.left += dx;
    label.top += dy;
    label.bottom += dy;
    int result;
    if (!view)
    {
        if (text.c_str())
            DrawTextW(dc, text.c_str(), -1, &label, 0x40015);
        result = CDRF_SKIPDEFAULT;
    }
    else if ((unsigned)(view - 2) <= 1)
    {
        label.left = labelLeft + 2;
        label.top = top + 2;
        if (text.c_str())
            DrawTextW(dc, text.c_str(), -1, &label, 0x40010);
        result = CDRF_SKIPDEFAULT;
    }
    else if (m_view == 4)
    {
        int lines = m_tileLines;
        label.right -= 6;
        label.top = (bottom - 13 * lines - top) / 2 + top;
        label.bottom = label.top + 13;
        SetTextColor(dc, 0);
        if (text.c_str())
            DrawTextW(dc, text.c_str(), -1, &label, 0x40000);
        SetTextColor(dc, 0x979797);
        for (int j = 1; j < m_tileLines; ++j)
        {
            label.top += 13;
            label.bottom += 13;
            if (j < 0 || j >= m_tileColumns.m_count)
                throw ArrayIndexOutOfBoundsException(j, m_tileColumns.m_count);
            String line = getCellText(index, m_tileColumns.m_items[j]);
            if (line.c_str())
                DrawTextW(dc, line.c_str(), -1, &label, 0x40000);
        }
        result = CDRF_SKIPDEFAULT;
    }
    else if (m_view != 1)
    {
        result = CDRF_SKIPDEFAULT;
    }
    else if (!loadColumnOrder(false))
    {
        for (int k = 0; k < m_columnCount; ++k)
        {
            int column;
            if (k < 0 || k >= m_columnOrder.m_count)
                column = 0;
            else
                column = m_columnOrder.m_items[k];
            RECT headerRc;
            SendMessageW(m_hHeader, HDM_GETITEMRECT, column, (LPARAM)&headerRc);
            LONG cellLeft = headerRc.left - m_scrollPos + 6;
            LONG cellRight = headerRc.right - m_scrollPos - 6;
            label.left = cellLeft;
            label.right = cellRight;
            if (!column)
            {
                cellLeft += icon.right - icon.left;
                label.left = cellLeft;
            }
            if (cellRight >= 0 && cellLeft <= client.right)
            {
                short format = *m_columnFormats.at(column);
                UINT align = format & 3;
                if ((format & 3) != 0)
                    align = 2 - (align != 1);
                if (node)
                {
                    long* columnId = m_columnIds.at(column);
                    const wchar_t* value = formatValue(node, *columnId, format);
                    bool bold = (format & LCF_BOLD) != 0;
                    if (bold)
                    {
                        HGDIOBJ old = SelectObject(dc, getStdFont(6));
                        if (!savedFont)
                            savedFont = old;
                        gdc.m_saved0C = savedFont;
                    }
                    if (value)
                        DrawTextW(dc, value, -1, &label, align | 0x8024);
                    if (bold)
                    {
                        HGDIOBJ old = SelectObject(dc, getStdFont(0));
                        if (!savedFont)
                        {
                            savedFont = old;
                            gdc.m_saved0C = old;
                        }
                    }
                }
                else
                {
                    String cell = getCellText(index, column);
                    if (cell.c_str())
                        DrawTextW(dc, cell.c_str(), -1, &label, align | 0x8024);
                }
            }
        }
        result = CDRF_SKIPDEFAULT;
    }
    else
    {
        result = 0;
    }
    return result;
}

HRESULT ListCtrl::onGetDispInfo(NMLVDISPINFOW* di)
{
    if ((di->item.mask & LVIF_COLUMNS) != 0)
    {
        di->item.cColumns = m_columnIds.m_count;
        di->item.puColumns = (PUINT)m_columnIds.m_items;
        return 0;
    }
    Node* node = getNode(di->item.iItem);
    if (!node)
    {
        OutputDebugStringW(L"*** Node is null ***\n");
        return E_FAIL;
    }
    if ((di->item.mask & LVIF_TEXT) != 0)
    {
        int subItem = di->item.iSubItem;
        if (subItem > m_columnIds.m_count)
            return E_FAIL;
        unsigned short* format = m_columnFormats.at(subItem);
        long* columnId = m_columnIds.at(di->item.iSubItem);
        di->item.pszText = (LPWSTR)formatValue(node, *columnId, *format);
    }
    if ((di->item.mask & LVIF_IMAGE) != 0)
    {
        long id = node->getId();
        di->item.iImage = m_imageMap.get(id, 0);
    }
    return 0;
}

LRESULT ListCtrl::windowProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_NCPAINT:
    {
        LRESULT result = DefSubclassProc(m_hWnd, WM_NCPAINT, wParam, lParam);
        HWND parent = GetParent(m_hWnd);
        if ((GetWindowLongW(parent, GWL_STYLE) & WS_BORDER) == 0)
            return result;
        WindowDC wdc(m_hWnd);
        HDC hdc = wdc.m_hdc;
        SIZE size;
        SIZE* s = getSize(&size);
        int x = s->cx - 1;
        int y = s->cy - 1;
        MoveToEx(hdc, 0, 0, 0);
        wdc.m_savedPen = SelectObject(hdc, CreatePen(PS_SOLID, 1, 0xB3ADAB));
        LineTo(hdc, x, 0);
        HGDIOBJ old = SelectObject(hdc, CreatePen(PS_SOLID, 1, 0xD6CBC5));
        if (wdc.m_savedPen)
            DeleteObject(old);
        else
            wdc.m_savedPen = old;
        LineTo(hdc, x, y);
        LineTo(hdc, 0, y);
        LineTo(hdc, 0, 0);
        return result;
    }
    case WM_PAINT:
    {
        PaintDC pdc(m_hWnd);
        DefSubclassProc(m_hWnd, WM_PRINTCLIENT, (WPARAM)pdc.m_hdc, PRF_CLIENT);
        if (m_highlight)
        {
            if (g_dragImage && g_dragImage->m_visible)
            {
                BitBlt(g_dragImage->m_hdcScreen, g_dragImage->m_pos.x, g_dragImage->m_pos.y,
                       g_dragImage->m_size.cx, g_dragImage->m_size.cy, g_dragImage->m_hdcMem, 0, 0, SRCCOPY);
                g_dragImage->m_visible = false;
            }
            RECT rc;
            getClientRect(&rc);
            ++rc.left;
            rc.top += m_headerHeight;
            HGDIOBJ old = SelectObject(pdc.m_hdc, CreatePen(PS_SOLID, 2, 0xFFB9B9));
            if (pdc.m_savedPen)
                DeleteObject(old);
            else
                pdc.m_savedPen = old;
            pdc.frameRect(&rc);
            if (g_dragImage && !g_dragImage->m_visible)
            {
                BitBlt(g_dragImage->m_hdcMem, 0, 0, g_dragImage->m_size.cx, g_dragImage->m_size.cy,
                       g_dragImage->m_hdcScreen, g_dragImage->m_pos.x, g_dragImage->m_pos.y, SRCCOPY);
                drawBitmapTransparent(g_dragImage->m_hdcScreen, &g_dragImage->m_pos, g_dragImage->m_hbmImage);
                g_dragImage->m_visible = true;
            }
        }
        return 0;
    }
    case WM_MOUSELEAVE:
    {
        int hot = m_hotItem;
        m_trackingMouse = false;
        if (hot != -1)
        {
            RECT rc;
            InvalidateRect(m_hWnd, getItemRect(&rc, hot, 0), TRUE);
            m_hotItem = -1;
        }
        break;
    }
    case 0x407:
        return getHeaderMenu();
    case 0x40C:
        return getItemMenu();
    case 0x408:
        onContextMenu((HMENU)wParam);
        return 0;
    }
    return DefSubclassProc(m_hWnd, msg, wParam, lParam);
}

bool ListCtrl::deleteSelected(const wchar_t* )
{
    getCount(0);
    if (getCount(1) != 1)
        return false;
    int index = getNextSelected(0);
    if (index == -1)
        return false;
    selectItem(-1);
    if (!removeItem(index))
    {
        selectItem(index);
        return false;
    }
    deleteItem(index);
    selectItem(index);
    onChanged();
    return true;
}

bool ListCtrl::activateItem(int index, int keys)
{
    HWND parent = GetParent(m_hWnd);
    if (SendMessageW(parent, 0x40B, index, keys))
        return true;
    return onActivateItem(index, keys);
}

LRESULT CALLBACK ListCtrl::editSubclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                            UINT_PTR , DWORD_PTR refData)
{
    ListCtrl* list = (ListCtrl*)refData;
    if (!list)
        return DefSubclassProc(hWnd, msg, wParam, lParam);
    if (msg != WM_SHOWWINDOW)
    {
        if (msg == WM_WINDOWPOSCHANGING && !list->m_editMoving)
            ((WINDOWPOS*)lParam)->flags |= SWP_NOSIZE | SWP_NOMOVE;
        return DefSubclassProc(hWnd, msg, wParam, lParam);
    }
    if (!wParam)
    {
        list->m_editNode = 0;
        return DefSubclassProc(hWnd, msg, wParam, lParam);
    }
    list->m_editMoving = true;
    SetWindowPos(hWnd, 0, list->m_editRect.left, list->m_editRect.top,
                 list->m_editRect.right - list->m_editRect.left, list->m_editRect.bottom - list->m_editRect.top,
                 SWP_NOZORDER | SWP_FRAMECHANGED);
    Node* node = list->m_editNode;
    list->m_editMoving = false;
    const wchar_t* text;
    if (node)
    {
        text = list->formatValue(node, list->m_editColumnId, list->m_editFormat);
    }
    else
    {
        if (!list->onGetDispInfo(&g_listEditDispInfo))
        {
            SetWindowTextW(hWnd, g_listEditDispInfo.item.pszText);
            return DefSubclassProc(hWnd, WM_SHOWWINDOW, wParam, lParam);
        }
        LVITEMW item;
        wchar_t buffer[256];
        memset(&item.iItem, 0, sizeof(item) - sizeof(item.mask));
        memset(buffer, 0, sizeof(buffer));
        item.mask = LVIF_TEXT;
        item.iItem = list->getNextSelected(0);
        item.iSubItem = list->m_editColumn;
        item.pszText = buffer;
        item.cchTextMax = 256;
        if (!SendMessageW(list->m_hWnd, LVM_GETITEMW, 0, (LPARAM)&item))
            return DefSubclassProc(hWnd, WM_SHOWWINDOW, wParam, lParam);
        text = buffer;
        SetWindowTextW(hWnd, text);
        return DefSubclassProc(hWnd, WM_SHOWWINDOW, wParam, lParam);
    }
    SetWindowTextW(hWnd, text);
    return DefSubclassProc(hWnd, WM_SHOWWINDOW, wParam, lParam);
}

int ListCtrl::getColumnAtCursor(int index, int* range)
{
    RECT rc;
    getItemRect(&rc, index, 0);
    POINT pt;
    pt.x = 0;
    pt.y = 0;
    GetCursorPos(&pt);
    pt.x -= rc.left;
    return columnFromPoint(&pt, (LONG*)range, 0);
}

bool ListCtrl::onBeginLabelEdit(NMLVDISPINFOW* di)
{
    m_editNode = getNode(di->item.iItem);
    RECT rc;
    getItemRect(&rc, di->item.iItem, 0);
    HWND edit = (HWND)SendMessageW(m_hWnd, LVM_GETEDITCONTROL, 0, 0);
    SendMessageW(edit, WM_GETFONT, 0, 0);
    int range[2];
    range[0] = 0;
    range[1] = 0;
    int column = getColumnAtCursor(di->item.iItem, range);
    if (!edit || column == -1 || (*m_columnFormats.at(column) & LCF_BOLD) != 0)
        return true;
    if (!m_editNode)
    {
        g_listEditDispInfo.hdr.hwndFrom = m_hWnd;
        g_listEditDispInfo.hdr.idFrom = 101;
        g_listEditDispInfo.item.iItem = di->item.iItem;
        g_listEditDispInfo.item.iSubItem = column;
    }
    m_editColumn = column;
    m_editColumnId = *m_columnIds.at(column);
    SetRect(&m_editRect, rc.left + range[0], rc.top - 1, rc.left + range[1] + 1, rc.bottom);
    m_editMoving = true;
    unsigned short kind = *m_columnFormats.at(column) & LCF_VALUE_MASK;
    m_editFormat = kind;
    m_editCancelled = false;
    if (kind == LCF_SIGNED)
        SetWindowLongW(edit, GWL_STYLE, GetWindowLongW(edit, GWL_STYLE) | ES_NUMBER);
    SetWindowSubclass(edit, editSubclassProc, 'edit', (DWORD_PTR)this);
    return false;
}

bool ListCtrl::onEndLabelEdit(NMLVDISPINFOW* di)
{
    wchar_t* text = di->item.pszText;
    if (!text || m_editCancelled)
    {
        updateItem(di->item.iItem);
        return false;
    }
    Node* node = getNode(di->item.iItem);
    m_editNode = node;
    bool ok;
    if (node)
    {
        unsigned int value = 0;
        switch (m_editFormat)
        {
        case 0:
            value = (unsigned int)(UINT_PTR)text;
            break;
        case LCF_SIGNED:
        case LCF_UNSIGNED:
            value = _wtoi(text);
            break;
        case LCF_HEX:
        {
            String hex(text, (unsigned)-1);
            value = hex.toHex();
            break;
        }
        case LCF_FLOAT:
        {
            float f = (float)_wtof(text);
            memcpy(&value, &f, sizeof(value));
            break;
        }
        default:
            break;
        }
        if (m_editFormat)
            ok = m_editNode->setInt(m_editColumnId, (int)value);
        else
            ok = m_editNode->setString(m_editColumnId, text);
    }
    else
    {
        ok = setCellText(di->item.iItem, m_editColumn, text);
    }
    if (ok)
    {
        onChanged();
        int next = di->item.iItem + 1;
        selectItem(next);
        PostMessageW(m_hWnd, LVM_EDITLABELW, next, 0);
        return true;
    }
    return false;
}
