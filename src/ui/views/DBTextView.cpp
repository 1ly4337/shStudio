#include "DBTextView.h"
#include "DBTextTable.h"
#include "FileTree.h"
#include "SearchPanel.h"
#include "AlertDial.h"
#include "String.hpp"
#include <windowsx.h>
#include <wchar.h>

static const int kListId = 101;
static const UINT kMsgEndEdit = WM_APP + 0x50;
static WPARAM endEditArg(bool commit, int move) { return (WPARAM)((commit ? 1 : 0) | ((move + 1) << 1)); }
extern HWND g_hMainWnd;

DBTextView::DBTextView(FileTree* file)
    : DocView(file, 0)
    , m_table(0)
    , m_list(0)
    , m_edit(0)
    , m_editRow(-1)
    , m_editField(-1)
    , m_curField(-1)
{
    DBTextTable* table = new DBTextTable();
    m_table = table;
    m_content = table;
    m_object = static_cast<Serializable*>(table);
    if (file)
        table->setCodePage((int)DBTextTable::codePageForName(file->m_name.m_buffer));
}

DBTextView::DBTextView(FileTree* file, Serializable* content, TableModel* model)
    : DocView(file, 0)
    , m_table(model)
    , m_list(0)
    , m_edit(0)
    , m_editRow(-1)
    , m_editField(-1)
    , m_curField(-1)
{
    m_content = content;
    m_object = content;
}

std::wstring DBTextView::columnTitle(int field)
{
    return m_table->fieldName(field);
}

bool DBTextView::save()
{
    if (m_edit)
        endEdit(true, 0);
    std::wstring error;
    bool reordered = false;
    if (!m_table->prepareSave(error, &reordered))
    {
        AlertDial::alert(m_hWnd, error.c_str(), L"Error", MB_ICONHAND);
        return false;
    }
    if (reordered && m_list)
        InvalidateRect(m_list, 0, FALSE);
    return DocView::save();
}

DBTextView::~DBTextView()
{
}

COMDLG_FILTERSPEC* DBTextView::getFilters(int* count)
{
    static COMDLG_FILTERSPEC filters[] = { { L"SData File", L"*.sdata" } };
    *count = 1;
    return filters;
}

HRESULT DBTextView::load()
{
    HRESULT hr = DocView::load();
    if (hr && hr != E_ABORT && !m_table->error().empty())
    {
        std::wstring text = L"The file ";
        if (m_data)
            text += m_data->m_name.m_buffer ? m_data->m_name.m_buffer : L"";
        text += L" cannot be opened as ";
        text += m_table->typeName();
        text += L".\n";
        text += m_table->error();
        AlertDial::alert(g_hMainWnd, text.c_str(), L"Error", MB_ICONHAND);
    }
    return hr;
}

bool DBTextView::onCreate()
{
    if (!DocView::onCreate())
        return false;
    RECT rc;
    getContentRect(&rc);
    m_list = CreateWindowExW(WS_EX_CLIENTEDGE, WC_LISTVIEWW, L"",
                             WS_CHILD | WS_VISIBLE | WS_TABSTOP | LVS_REPORT | LVS_OWNERDATA | LVS_SHOWSELALWAYS,
                             rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
                             m_hWnd, (HMENU)(INT_PTR)kListId, GetModuleHandleW(0), 0);
    if (!m_list)
        return false;
    SendMessageW(m_list, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), FALSE);
    ListView_SetExtendedListViewStyle(m_list, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);
    fillColumns();
    refresh(0);
    showStatus();
    SetFocus(m_list);
    return true;
}

void DBTextView::fillColumns()
{
    int n = m_table->fieldCount();
    HDC dc = GetDC(m_list);
    HGDIOBJ oldFont = SelectObject(dc, (HGDIOBJ)SendMessageW(m_list, WM_GETFONT, 0, 0));
    for (int f = 0; f < n; ++f)
    {
        std::wstring name = m_table->fieldName(f);
        std::wstring title = columnTitle(f);
        bool numeric = m_table->isNumericField(f);
        LVCOLUMNW col = {};
        col.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT | LVCF_SUBITEM;
        col.fmt = numeric ? LVCFMT_RIGHT : LVCFMT_LEFT;
        col.cx = numeric ? 80 : (!_wcsicmp(name.c_str(), L"text") ? 420 : 220);
        SIZE extent = {};
        if (numeric && GetTextExtentPoint32W(dc, title.c_str(), (int)title.size(), &extent) && extent.cx + 24 > col.cx)
            col.cx = extent.cx + 24;
        col.pszText = (LPWSTR)title.c_str();
        col.iSubItem = f;
        SendMessageW(m_list, LVM_INSERTCOLUMNW, f, (LPARAM)&col);
    }
    SelectObject(dc, oldFont);
    ReleaseDC(m_list, dc);
    m_curField = m_table->keyCount() < n ? m_table->keyCount() : 0;
}

void DBTextView::refresh(int focus)
{
    SendMessageW(m_list, LVM_SETITEMCOUNT, m_table->rowCount(), LVSICF_NOSCROLL);
    InvalidateRect(m_list, 0, FALSE);
    if (focus >= 0 && focus < m_table->rowCount())
        selectRow(focus);
}

int DBTextView::focusedRow() const
{
    return (int)SendMessageW(m_list, LVM_GETNEXTITEM, (WPARAM)-1, LVNI_FOCUSED);
}

void DBTextView::selectRow(int row)
{
    ListView_SetItemState(m_list, -1, 0, LVIS_SELECTED);
    ListView_SetItemState(m_list, row, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
    ListView_EnsureVisible(m_list, row, FALSE);
}

void DBTextView::showStatus()
{
    setStatusText(L"%s", (INT_PTR)m_table->statusText().c_str());
}

bool DBTextView::onNotify(NMHDR* hdr, LRESULT* result)
{
    if (!hdr || hdr->hwndFrom != m_list)
        return DocView::onNotify(hdr, result);
    switch (hdr->code)
    {
    case LVN_GETDISPINFOW:
    {
        NMLVDISPINFOW* di = (NMLVDISPINFOW*)hdr;
        if ((di->item.mask & LVIF_TEXT) && di->item.pszText && di->item.cchTextMax > 0)
        {
            m_dispBuf = m_table->getText(di->item.iItem, di->item.iSubItem);
            for (wchar_t& c : m_dispBuf)
                if (c == L'\r' || c == L'\n' || c == L'\t')
                    c = (c == L'\n') ? 0xB6 : L' ';
            wcsncpy_s(di->item.pszText, di->item.cchTextMax, m_dispBuf.c_str(), _TRUNCATE);
        }
        *result = 0;
        return true;
    }
    case NM_CLICK:
    case NM_DBLCLK:
    {
        NMITEMACTIVATE* ia = (NMITEMACTIVATE*)hdr;
        LVHITTESTINFO ht = {};
        ht.pt = ia->ptAction;
        SendMessageW(m_list, LVM_SUBITEMHITTEST, 0, (LPARAM)&ht);
        if (ht.iSubItem >= 0 && ht.iSubItem < m_table->fieldCount())
            m_curField = ht.iSubItem;
        if (hdr->code == NM_DBLCLK && ht.iItem >= 0)
            beginEdit(ht.iItem, m_curField);
        *result = 0;
        return true;
    }
    case LVN_KEYDOWN:
    {
        NMLVKEYDOWN* kd = (NMLVKEYDOWN*)hdr;
        bool shift = GetKeyState(VK_SHIFT) < 0;
        if (kd->wVKey == VK_F2)
            beginEdit(focusedRow(), m_curField);
        else if (kd->wVKey == VK_RETURN)
            beginEdit(focusedRow(), m_curField);
        else if (kd->wVKey == VK_DELETE && !shift)
            deleteSelected();
        else if (kd->wVKey == VK_LEFT && GetKeyState(VK_CONTROL) < 0 && m_curField > 0)
            --m_curField;
        else if (kd->wVKey == VK_RIGHT && GetKeyState(VK_CONTROL) < 0 && m_curField + 1 < m_table->fieldCount())
            ++m_curField;
        *result = 0;
        return true;
    }
    }
    return false;
}

bool DBTextView::onCommand(UINT code, HWND ctrl, UINT id)
{
    if ((code | 1) == 1 && !ctrl)
    {
        if (m_edit)
        {
            switch (id)
            {
            case 0xE122: SendMessageW(m_edit, WM_CUT, 0, 0); return true;
            case 0xE123: SendMessageW(m_edit, WM_COPY, 0, 0); return true;
            case 0xE124: SendMessageW(m_edit, WM_PASTE, 0, 0); return true;
            case 0xE125: SendMessageW(m_edit, EM_SETSEL, 0, -1); return true;
            case 0xE120: SendMessageW(m_edit, EM_UNDO, 0, 0); return true;
            case 0xE12A: case 0xE12B: case 0xE12C: return true;
            case 0xE103: case 0xE102: endEdit(true, 0); break;
            }
        }
        switch (id)
        {
        case 0xE12A: insertRow(); return true;
        case 0xE12B: deleteSelected(); return true;
        case 0xE12C: eraseSelected(); return true;
        case 0xE123: copySelected(); return true;
        case 0xE125:
            ListView_SetItemState(m_list, -1, LVIS_SELECTED, LVIS_SELECTED);
            return true;
        }
    }
    return DocView::onCommand(code, ctrl, id);
}

LRESULT DBTextView::defWindowProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == kMsgEndEdit)
    {
        if (m_edit && (HWND)lParam == m_edit)
            endEdit((wParam & 1) != 0, (int)(wParam >> 1) - 1);
        return 0;
    }
    if (msg == WM_SETFOCUS && m_list)
    {
        SetFocus(m_edit ? m_edit : m_list);
        return 0;
    }
    return DocView::defWindowProc(msg, wParam, lParam);
}

void DBTextView::beginEdit(int row, int field)
{
    if (m_edit)
        endEdit(true, 0);
    if (row < 0 || row >= m_table->rowCount() || field < 0 || field >= m_table->fieldCount())
        return;
    m_curField = field;
    ListView_EnsureVisible(m_list, row, FALSE);
    RECT rc;
    ListView_GetSubItemRect(m_list, row, field, field ? LVIR_BOUNDS : LVIR_LABEL, &rc);
    RECT client;
    GetClientRect(m_list, &client);
    if (rc.left < 0 || rc.right > client.right)
    {
        int dx = rc.left < 0 ? rc.left : (rc.right - client.right < rc.left ? rc.right - client.right : rc.left);
        ListView_Scroll(m_list, dx, 0);
        ListView_GetSubItemRect(m_list, row, field, field ? LVIR_BOUNDS : LVIR_LABEL, &rc);
    }
    if (rc.right > client.right)
        rc.right = client.right;
    std::wstring text = m_table->getText(row, field);
    std::wstring shown;
    for (size_t i = 0; i < text.size(); ++i)
    {
        if (text[i] == L'\n' && (i == 0 || text[i - 1] != L'\r'))
            shown += L'\r';
        shown += text[i];
    }
    DWORD style = WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_MULTILINE;
    if (m_table->isNumericField(field))
        style |= ES_RIGHT;
    m_edit = CreateWindowExW(0, L"EDIT", shown.c_str(), style, rc.left, rc.top - 1, rc.right - rc.left,
                             rc.bottom - rc.top + 2, m_list, 0, GetModuleHandleW(0), 0);
    if (!m_edit)
        return;
    m_editRow = row;
    m_editField = field;
    SendMessageW(m_edit, WM_SETFONT, SendMessageW(m_list, WM_GETFONT, 0, 0), FALSE);
    SetWindowSubclass(m_edit, editProc, 1, (DWORD_PTR)this);
    SendMessageW(m_edit, EM_SETSEL, 0, -1);
    SetFocus(m_edit);
}

void DBTextView::endEdit(bool commit, int move)
{
    if (!m_edit)
        return;
    HWND edit = m_edit;
    m_edit = 0;
    int row = m_editRow, field = m_editField;
    if (commit)
    {
        int len = GetWindowTextLengthW(edit);
        std::wstring text(len + 1, L'\0');
        GetWindowTextW(edit, &text[0], len + 1);
        text.resize(len);
        std::wstring orig = m_table->getText(row, field);
        if (orig.find(L"\r\n") == std::wstring::npos && orig.find(L'\n') != std::wstring::npos)
        {
            std::wstring lf;
            for (wchar_t c : text)
                if (c != L'\r')
                    lf += c;
            text.swap(lf);
        }
        int r = m_table->setText(row, field, text.c_str());
        if (r == DBTextTable::kChanged)
        {
            setModified(true);
            ListView_RedrawItems(m_list, row, row);
        }
        else if (r == DBTextTable::kBadNumber)
            AlertDial::alert(m_hWnd, m_table->isKeyField(field)
                                         ? L"This column holds an integer key: enter a decimal number."
                                         : L"This column holds an integer: enter a decimal number.",
                             L"Error", MB_ICONHAND);
        else if (r == DBTextTable::kNotEncodable)
            AlertDial::alert(m_hWnd, m_table->notEncodableText().c_str(), L"Error", MB_ICONHAND);
    }
    RemoveWindowSubclass(edit, editProc, 1);
    DestroyWindow(edit);
    SetFocus(m_list);
    if (move)
    {
        int next = field + move;
        if (next >= 0 && next < m_table->fieldCount())
            beginEdit(row, next);
    }
}

LRESULT CALLBACK DBTextView::editProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR id, DWORD_PTR ref)
{
    DBTextView* view = (DBTextView*)ref;
    switch (msg)
    {
    case WM_GETDLGCODE:
        return DLGC_WANTALLKEYS | DefSubclassProc(hWnd, msg, wParam, lParam);
    case WM_KEYDOWN:
        if (wParam == VK_RETURN && GetKeyState(VK_CONTROL) >= 0)
        {
            PostMessageW(view->m_hWnd, kMsgEndEdit, endEditArg(true, 0), (LPARAM)hWnd);
            return 0;
        }
        if (wParam == VK_ESCAPE)
        {
            PostMessageW(view->m_hWnd, kMsgEndEdit, endEditArg(false, 0), (LPARAM)hWnd);
            return 0;
        }
        if (wParam == VK_TAB)
        {
            PostMessageW(view->m_hWnd, kMsgEndEdit, endEditArg(true, GetKeyState(VK_SHIFT) < 0 ? -1 : 1), (LPARAM)hWnd);
            return 0;
        }
        break;
    case WM_CHAR:
        if (wParam == L'\t' || wParam == 0x1B)
            return 0;
        if (wParam == L'\r' || wParam == L'\n')
        {
            if (GetKeyState(VK_CONTROL) >= 0)
                return 0;
            SendMessageW(hWnd, EM_REPLACESEL, TRUE, (LPARAM)L"\r\n");
            return 0;
        }
        break;
    case WM_KILLFOCUS:
        if (view->m_edit == hWnd)
            PostMessageW(view->m_hWnd, kMsgEndEdit, endEditArg(true, 0), (LPARAM)hWnd);
        break;
    }
    return DefSubclassProc(hWnd, msg, wParam, lParam);
}

void DBTextView::insertRow()
{
    int focus = focusedRow();
    int at = m_table->insertRow(focus >= 0 ? focus + 1 : m_table->rowCount());
    if (at < 0)
        return;
    setModified(true);
    refresh(at);
    showStatus();
}

void DBTextView::deleteSelected()
{
    int count = ListView_GetSelectedCount(m_list);
    if (count <= 0)
        return;
    wchar_t msg[120];
    swprintf_s(msg, count == 1 ? L"Delete the selected row?" : L"Delete the %d selected rows?", count);
    if (AlertDial::alert(m_hWnd, msg, 0, MB_ICONQUESTION | MB_YESNO) != IDYES)
        return;
    int first = -1;
    for (int i = m_table->rowCount() - 1; i >= 0; --i)
        if (ListView_GetItemState(m_list, i, LVIS_SELECTED) & LVIS_SELECTED)
        {
            m_table->deleteRow(i);
            first = i;
        }
    ListView_SetItemState(m_list, -1, 0, LVIS_SELECTED | LVIS_FOCUSED);
    setModified(true);
    refresh(first < m_table->rowCount() ? first : m_table->rowCount() - 1);
    showStatus();
}

void DBTextView::eraseSelected()
{
    bool changed = false;
    for (int i = -1; (i = (int)SendMessageW(m_list, LVM_GETNEXTITEM, i, LVNI_SELECTED)) >= 0;)
    {
        for (int f = m_table->keyCount(); f < m_table->fieldCount(); ++f)
            if (!m_table->isNumericField(f) && m_table->setText(i, f, L"") == DBTextTable::kChanged)
                changed = true;
        ListView_RedrawItems(m_list, i, i);
    }
    if (changed)
        setModified(true);
}

void DBTextView::copySelected()
{
    std::wstring text;
    for (int i = -1; (i = (int)SendMessageW(m_list, LVM_GETNEXTITEM, i, LVNI_SELECTED)) >= 0;)
    {
        for (int f = 0; f < m_table->fieldCount(); ++f)
        {
            if (f)
                text += L'\t';
            std::wstring cell = m_table->getText(i, f);
            for (wchar_t& c : cell)
                if (c == L'\t' || c == L'\r' || c == L'\n')
                    c = L' ';
            text += cell;
        }
        text += L"\r\n";
    }
    if (text.empty() || !OpenClipboard(m_hWnd))
        return;
    EmptyClipboard();
    size_t bytes = (text.size() + 1) * sizeof(wchar_t);
    HGLOBAL mem = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (mem)
    {
        memcpy(GlobalLock(mem), text.c_str(), bytes);
        GlobalUnlock(mem);
        if (!SetClipboardData(CF_UNICODETEXT, mem))
            GlobalFree(mem);
    }
    CloseClipboard();
}

void DBTextView::findNext()
{
    String what = m_search.getText();
    if (!what.m_buffer || !what.m_length)
        return;
    std::wstring needle(what.m_buffer, what.m_length);
    CharLowerBuffW(&needle[0], (DWORD)needle.size());
    int n = m_table->rowCount();
    int start = focusedRow();
    for (int step = 1; step <= n; ++step)
    {
        int row = (start + step + n) % n;
        for (int f = 0; f < m_table->fieldCount(); ++f)
        {
            std::wstring cell = m_table->getText(row, f);
            if (cell.empty())
                continue;
            CharLowerBuffW(&cell[0], (DWORD)cell.size());
            if (cell.find(needle) != std::wstring::npos)
            {
                m_curField = f;
                selectRow(row);
                m_search.setStatus(L"");
                return;
            }
        }
    }
    m_search.setStatus(L"Not found");
}
