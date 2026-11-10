#include "ItemView.h"
#include "CountEditor.h"
#include "GradeEditor.h"
#include "Data_Item.h"
#include "Data_Items.h"
#include "Data_Item_fields_B.h"
#include "Clipboard.h"
#include "String.hpp"
#include "_imports_A.h"
#include <cstring>
#include "Collection.hpp"

COMDLG_FILTERSPEC g_itemFilters[3] = {
    { L"Episo 5 SData File", L"*.sdata" },
    { L"Episo 6 SData File", L"*.sdata" },
    { L"Episo 6.2 SData File", L"*.sdata" },
};

static BOOL setMenuItem(HMENU menu, UINT id, UINT mask, UINT state, const wchar_t* text, UINT length)
{
    MENUITEMINFOW mii;
    memset(&mii.fMask, 0, 0x2C);
    mii.cbSize = sizeof(MENUITEMINFOW);
    mii.fMask = mask;
    mii.fState = state;
    if (mask & MIIM_STRING)
    {
        mii.dwTypeData = (LPWSTR)text;
        mii.cch = length;
    }
    return SetMenuItemInfoW(menu, id, FALSE, &mii);
}

ItemView::ItemView(void* file, Serializable* data)
    : SDataView(file, data), m_tabs(this)
{
    Data::Items* items;
    if (data)
    {
        items = dynamic_cast<Data::Items*>(data);
    }
    else
    {
        items = new Data::Items();
        m_content = items;
        m_object = (Serializable*)items;
    }
    m_items = items;
    m_tabs.m_items = items;
}

ItemView::~ItemView()
{
}

bool ItemView::onCreate()
{
    if (!m_items)
        return false;
    if (!SDataView::onCreate())
        return false;
    HWND parent = m_hWnd;
    Data::Items* items = m_items;
    RECT rcBuf;
    RECT* rc = getClientRect(&rcBuf);
    m_tabs.m_items = items;
    if (!m_tabs.create(parent, (HMENU)0x65, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
                           WS_CHILD | WS_VISIBLE, 0, 0))
        return false;
    m_tabs.reloadAll();
    return true;
}

void ItemView::onDestroy()
{
    if (m_items)
        static_cast<ptrArray<Data::Item*>*>(m_items)->deleteAll();
    m_tabs.reloadAll();
    SDataView::onDestroy();
}

void ItemView::onActivate(bool active)
{
    m_tabs.onActivate(active);
}

bool ItemView::onCommand(UINT code, HWND ctrl, UINT id)
{
    if (id == 0xE410)
    {
        CountEditor editor;
        if (editor.create(m_hWnd) && editor.doModal() == 1)
        {
            int type = editor.m_type;
            if ((unsigned)(type - 1) <= 99)
            {
                unsigned char count = (unsigned char)editor.m_count;
                if ((unsigned)(editor.m_count - 1) <= 0xFE)
                {
                    if (editor.m_typeId)
                    {
                        Data::Item* item = m_items->Data::Items::sub_42FC00(type, editor.m_typeId);
                        if (item)
                            DI_Count(item) = count;
                    }
                    else
                    {
                        for (int i = 1; i <= 255; ++i)
                        {
                            Data::Item* item = m_items->Data::Items::sub_42FC00(type, i);
                            if (item)
                                DI_Count(item) = count;
                        }
                    }
                }
            }
            setModified(true);
        }
        return true;
    }
    if (id != 0xE411)
        return SDataView::onCommand(code, ctrl, id);

    GradeEditor editor;
    if (editor.create(m_hWnd) && editor.doModal() == 1)
    {
        m_tabs.setGrade((short)editor.m_grade);
        setModified(true);
    }
    return true;
}

bool ItemView::onInitMenu(HMENU menu)
{
    SDataView::onInitMenu(menu);
    m_tabs.onInitMenu((WPARAM)menu);
    return true;
}

bool ItemView::onNotify(NMHDR* hdr, LRESULT* result)
{
    if (hdr->code != LVN_KEYDOWN)
        return false;
    WORD vk = ((NMLVKEYDOWN*)hdr)->wVKey;
    GetKeyState(VK_CONTROL);
    if (GetKeyState(VK_CONTROL) < 0 && (vk == VK_PRIOR || vk == VK_NEXT))
    {
        LRESULT current = SendMessageW(m_tabs.m_hWnd, TCM_GETCURSEL, 0, 0);
        int index = (vk == VK_PRIOR) ? (int)current - 1 : (int)current + 1;
        LRESULT count = SendMessageW(m_tabs.m_hWnd, TCM_GETITEMCOUNT, 0, 0);
        if (index >= 0)
        {
            if (index >= count)
                index = 0;
        }
        else
        {
            index = (int)count - 1;
        }
        m_tabs.select(index);
    }
    return true;
}

void ItemView::updateMenu(HMENU menu)
{
    SDataView::updateMenu(menu);
    setMenuItem(menu, 0xE12A, MIIM_STATE, 0, 0, 0);
    setMenuItem(menu, 0xE12B, MIIM_STATE, MFS_GRAYED | MFS_DISABLED, 0, 0);
    setMenuItem(menu, 0xE12C, MIIM_STATE, 0, 0, 0);
    setMenuItem(menu, 0xE402, MIIM_STATE | MIIM_STRING, 0, L"Episo 5 format (82 bytes per row)", 33);
    setMenuItem(menu, 0xE403, MIIM_STATE | MIIM_STRING, 0, L"Episo 6 format (132 bytes per row)", 34);
    setMenuItem(menu, 0xE404, MIIM_STATE | MIIM_STRING, 0, L"Episo 6.2 format (134 bytes per row)", 36);
}

void ItemView::fillScriptMenu(HMENU menu)
{
    Popup::appendMenuItem(menu, 0xE410);
    Popup::appendMenuItem(menu, 0xE411);
    Popup::appendMenuItem(menu, 0xE435);
    Popup::appendMenuItem(menu, 0xE430);
    Popup::appendMenuItem(menu, 0xE431);
}

LPCWSTR ItemView::getCommandHelp(UINT id)
{
    switch (id)
    {
    case 0xE410: return L"Set Count value";
    case 0xE411: return L"Set Grade value";
    case 0xE430: return L"Import text fields from a language file";
    case 0xE431: return L"Import definition from database";
    }
    const wchar_t* text = L"Change codepage...";
    if (id != 0xE435)
        return 0;
    return text;
}

void ItemView::setFilterIndex(int index)
{
    switch (index)
    {
    case 1:
        m_items->m_value30 = (m_items->m_value30 & 0xFFFFF000) | 0x500;
    case 2:
        m_items->m_value30 = (m_items->m_value30 & 0xFFFFF000) | 0x600;
    case 3:
        m_items->m_value30 = (m_items->m_value30 & 0xFFFFF000) | 0x620;
        break;
    }
}

int ItemView::getFilterIndex()
{
    switch (m_items->m_value30 & 0xFFF)
    {
    case 0x500: return 1;
    case 0x600: return 2;
    case 0x620: return 3;
    }
    return 0;
}

COMDLG_FILTERSPEC* ItemView::getFilters(int* count)
{
    *count = 3;
    return g_itemFilters;
}

String ItemView::getMergeFilter()
{
    String filter;
    filter.append(L"Items SData File\tDBItemText_*.SData\t", (unsigned)-1);
    return filter;
}

void ItemView::onDataUpdated()
{
    setStatusText(0, 0);
    setModified(true);
    m_tabs.reloadAll();
}

bool ItemView::exportData(String* out, DWORD format)
{
    static_cast<Streamable*>(m_items)->toText(*out, format);
    return true;
}

const wchar_t* ItemView::getSQLTableName()
{
    return L"PS_GameDefs.dbo.Items";
}

void ItemView::importSQLRow(RecordSet* rs, bool captions)
{
    int itemId;
    void* fields = FIELD(void*, rs, 0x04);
    if (fields)
    {
        void* row = A_VCALL(fields, 0x04, void*)(fields);
        itemId = A_VCALL(row, 0x18, int, const wchar_t*)(row, L"ItemID");
    }
    else
    {
        itemId = 0;
    }
    Data::Item* item = m_items->Data::Items::sub_42FC00(itemId / 1000, itemId % 1000);
    if (!item)
    {
        ptrArray<Data::Item*>* all = m_items;
        item = new Data::Item(all->size(), m_items->m_value30);
        all->add(item);
    }
    item->readSQLRow(rs, captions);
}

void ItemView::setViewMode(int mode)
{
    if (mode)
    {
        m_items->setCodePage(mode);
        setModified(true);
    }
}

int ItemView::getViewMode()
{
    return m_items->m_value30 & 0xFFF;
}

HRESULT ItemView::copyToClipboard()
{
    if (!m_items)
        return E_POINTER;
    String text;
    text << L"Type;TypeID;Name;Description;Model;Icon;Level;Country;Class1;Class2;Class3;Class4;Class5;Class6;Grow;"
            L"Type2;Type3;ReqStr;ReqDex;ReqRec;ReqInt;ReqWis;ReqLuc;ReqVg;ReqOg;ReqIg;Range;AttackTime;Attrib;"
            L"Special;Slot;Quality;Effect1;Effect2;Effect3;Effect4;ConstHP;ConstSP;ConstMP;ConstStr;ConstDex;"
            L"ConstRec;ConstInt;ConstWis;ConstLuc;Speed;Exp;Buy;Sell;Grade;Drop;Server;Count"
         << L"\r\n";
    ptrArray<Data::Item*>* all = m_items;
    int count = all->m_count;
    if (count >= 2)
    {
        all->m_changes = 0;
        quickSort(*all, 0, count - 1, 'type', 'tpid', true);
    }
    int n = all->size();
    for (int i = 0; i < n; ++i)
        all->m_items[i]->toText(text, 'csve');

    wchar_t* buffer = text.m_buffer;
    if (buffer)
    {
        int length = (int)wcslen(buffer);
        if (length > 0)
        {
            if (OpenClipboard(g_clipboard.m_hOwner))
            {
                EmptyClipboard();
                SIZE_T size = 2 * length + 2;
                HGLOBAL global = GlobalAlloc(GMEM_MOVEABLE, size);
                if (global)
                {
                    void* dest = GlobalLock(global);
                    if (dest)
                        memcpy(dest, buffer, size);
                    GlobalUnlock(global);
                    SetClipboardData(CF_UNICODETEXT, global);
                }
                CloseClipboard();
            }
        }
    }
    return S_OK;
}
