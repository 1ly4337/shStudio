#include "GradeList.h"
#include "GradeSet.h"
#include "GradeSets.h"
#include "GradeManager.h"
#include "GetGradeDialog.h"
#include "ItemEditor.h"
#include "ItemView.h"
#include "Data_Items.h"
#include "Data_Item.h"
#include "Cell.h"
#include "GDC.h"
#include "DataObject.h"
#include "AlertDial.h"
#include "Bytes.h"
#include "String.hpp"
#include <typeinfo>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "_imports_p07.h"
#include "StudioFrame.h"
#include "ClientData.h"

static const CLIPFORMAT kItemRecordsFormat = 0x202;

GradeList::GradeList(ItemView* owner, Data::Items* data)
    : TreeList()
    , m_owner(owner)
    , m_data(data)
    , m_sets()
    , m_images()
{
    m_images.m_hList = 0;
    m_images.m_cx = 16;
    m_images.m_cy = 0;
    m_images.load(160, 0);
}

GradeList::~GradeList()
{
    m_64 = 0;
    SendMessageW(m_tree.m_hWnd, TVM_DELETEITEM, 0, (LPARAM)TVI_ROOT);
    m_sets.detachAll();
}

BOOL GradeList::show(bool show)
{
    if (show)
        refresh();
    return ShowWindow(m_hWnd, show ? SW_SHOW : SW_HIDE);
}

DWORD GradeList::getDetailsTag()
{
    return 'dtls';
}

bool GradeList::onCreate()
{
    if (!TreeList::onCreate())
        return false;

    SendMessageW(m_tree.m_hWnd, TVM_SETIMAGELIST, TVSIL_NORMAL, (LPARAM)m_images.m_hList);

    LVCOLUMNW column;
    memset(&column, 0, sizeof(column));
    column.mask = 7;
    column.fmt = 0x1000;
    column.cx = 250;
    column.pszText = (LPWSTR)L"Item";
    insertColumn(&column, 'name');

    memset(&column, 0, sizeof(column));
    column.mask = 7;
    column.fmt = m_columnCount ? 2 : 0;
    column.cx = 50;
    column.pszText = (LPWSTR)L"Idnt";
    insertColumn(&column, 'typS');

    memset(&column, 0, sizeof(column));
    column.mask = 7;
    column.fmt = m_columnCount ? 2 : 0;
    column.cx = 40;
    column.pszText = (LPWSTR)L"Cntr";
    insertColumn(&column, 'cnts');

    memset(&column, 0, sizeof(column));
    column.mask = 7;
    column.fmt = m_columnCount ? 0x12 : 0x10;
    column.cx = 40;
    column.pszText = (LPWSTR)L"Lvl";
    insertColumn(&column, 'lvl ');

    memset(&column, 0, sizeof(column));
    column.mask = 7;
    column.fmt = m_columnCount ? 0x12 : 0x10;
    column.cx = 40;
    column.pszText = (LPWSTR)L"Slot";
    insertColumn(&column, 'slot');

    memset(&column, 0, sizeof(column));
    column.mask = 7;
    column.fmt = 0;
    column.cx = 100;
    column.pszText = (LPWSTR)L"Info";
    insertColumn(&column, 'dtls');

    m_dropTarget = new GradeManager(this, &m_tree);
    return true;
}

LRESULT GradeList::deleteAllItems()
{
    m_64 = 0;
    LRESULT result = SendMessageW(m_tree.m_hWnd, TVM_DELETEITEM, 0, (LPARAM)TVI_ROOT);
    m_sets.detachAll();
    return result;
}

int GradeList::getNodeImage(Node* node)
{
    Data::Item* item = dynamic_cast<Data::Item*>(node);
    if (!item)
        return 2;
    long id = itemIconSheet(FIELD(BYTE, item, 0x54) ) * 1000 + FIELD(BYTE, item, 0x57) ;
    int index;
    for (index = 0; index < m_images.m_ids.m_capacity; ++index)
    {
        if (m_images.m_ids.m_items[index] == id)
            break;
    }
    if (index >= m_images.m_ids.m_capacity)
        index = -1;
    if (index < 0)
        return 2;
    return index;
}

void GradeList::refresh()
{
    deleteAllItems();
    int count = m_data->size();
    if (!count)
        return;

    for (int i = 0; i < count; ++i)
    {
        Data::Item* item = m_data->get(i);
        if (!item)
            continue;
        const String& name = FIELD(String, item, 0x14);
        if (name.m_buffer && name.m_length > 0 && name.m_buffer[0] == L'?')
            continue;
        if (FIELD(BYTE, item, 0x54)  <= 0x15 && FIELD(short, item, 0x58)  <= 0)
            continue;
        short grade = FIELD(short, item, 0xA4);
        if (!grade)
            continue;
        GradeSets* sets = &m_sets;
        GradeSet* set = m_sets.find(grade);
        if (!set)
        {
            set = new GradeSet((unsigned short)grade);
            sets->add(set);
        }
        set->add(item);
    }

    if (m_sets.m_count >= 2)
    {
        m_sets.m_changes = 0;
        m_sets.sort(0, m_sets.m_count - 1, 'grde', 'cnt ', true);
    }

    TVINSERTSTRUCTW root;
    memset(&root, 0, sizeof(root));
    root.hParent = TVI_ROOT;
    root.hInsertAfter = TVI_ROOT;
    root.item.pszText = (LPWSTR)L"Grades";
    root.item.iImage = 0;
    root.item.iSelectedImage = 0;
    root.item.mask = 0x63;
    root.item.cChildren = 100;
    m_64 = (int)SendMessageW(m_tree.m_hWnd, TVM_INSERTITEMW, 0, (LPARAM)&root);
    if (m_64)
    {
        int sets = m_sets.size();
        for (int i = 0; i < sets; ++i)
        {
            GradeSet* set = m_sets.m_items[i];
            int items = set->size();
            if (items > 0)
            {
                wchar_t text[16];
                swprintf_s(text, 16, L"- %d -", set->m_grade);
                HTREEITEM parent = (HTREEITEM)m_64;
                HTREEITEM after = TVI_LAST;
                if (parent == TVI_ROOT)
                    after = TVI_ROOT;
                TVINSERTSTRUCTW group;
                memset(&group, 0, sizeof(group));
                group.item.pszText = text;
                group.hInsertAfter = after;
                group.hParent = parent;
                group.item.iImage = 1;
                group.item.iSelectedImage = 1;
                group.item.mask = 0x63;
                group.item.cChildren = 255;
                set->m_hItem = (HTREEITEM)SendMessageW(m_tree.m_hWnd, TVM_INSERTITEMW, 0, (LPARAM)&group);
                int j = 0;
                do
                {
                    Data::Item* item = set->get(j);
                    HTREEITEM node = set->m_hItem;
                    if (item)
                        insertCell(new Cell(item), node, -1, 0, TVI_LAST);
                    ++j;
                } while (j < items);
            }
        }
    }
    SendMessageW(m_tree.m_hWnd, TVM_EXPAND, TVE_EXPAND, (LPARAM)m_64);
}

bool GradeList::isGroupItem(HTREEITEM item)
{
    return item != (HTREEITEM)m_64 && getItemCell(item) == 0;
}

bool GradeList::canMultiSelect(HTREEITEM item)
{
    Data::Item* data = dynamic_cast<Data::Item*>(getItemNode(item));
    if (data)
    {
        if (!m_anchor)
            return true;
        Data::Item* anchor = dynamic_cast<Data::Item*>(getItemNode(m_anchor));
        if (!anchor || FIELD(short, anchor, 0xA4) == FIELD(short, data, 0xA4))
            return true;
    }
    return false;
}

IDataObject* GradeList::createDataObject()
{
    int count = m_selection.m_count;
    if (!count)
        return 0;

    Bytes buffer(0, (UINT)0);
    buffer << count;
    for (int i = 0; i < count; ++i)
    {
        HTREEITEM selected = (i < 0 || i >= m_selection.m_capacity) ? 0 : m_selection.m_items[i];
        Data::Item* item = dynamic_cast<Data::Item*>(getItemNode(selected));
        if (!item)
            return 0;
        int format = FIELD(int, item, 0x10);
        FIELD(int, item, 0x10) = 0x600;
        item->write(buffer, 'clip');
        FIELD(int, item, 0x10) = format;
    }
    clearSelection(true);
    UpdateWindow(m_tree.m_hWnd);

    FORMATETC format;
    format.cfFormat = kItemRecordsFormat;
    format.ptd = 0;
    format.dwAspect = DVASPECT_CONTENT;
    format.lindex = 0;
    format.tymed = TYMED_HGLOBAL;
    STGMEDIUM medium;
    medium.tymed = TYMED_HGLOBAL;
    medium.pUnkForRelease = 0;
    HGLOBAL hMem = GlobalAlloc(GHND, buffer.m_size);
    void* dst = GlobalLock(hMem);
    memcpy(dst, buffer.m_data, buffer.m_size);
    GlobalUnlock(hMem);
    medium.hGlobal = hMem;
    return new DataObject(&format, &medium, 1);
}

void GradeList::dropSelection(HTREEITEM target, LPARAM data, int effect)
{
    if (!data || !effect)
        return;
    short grade = gradeOf(target);
    if (grade < 1)
        return;
    GradeSet* set = m_sets.find(grade);
    if (!set)
        return;
    int count = m_selection.m_count;
    for (int i = 0; i < count; ++i)
    {
        HTREEITEM selected = (i < 0 || i >= m_selection.m_capacity) ? 0 : m_selection.m_items[i];
        Data::Item* item = dynamic_cast<Data::Item*>(getItemNode(selected));
        if (item && FIELD(short, item, 0xA4) != grade)
        {
            GradeSet* old = m_sets.find(FIELD(short, item, 0xA4));
            if (old)
                old->detachItem(item);
            SendMessageW(m_tree.m_hWnd, TVM_DELETEITEM, 0, (LPARAM)selected);
            FIELD(short, item, 0xA4) = grade;
            set->add(item);
            insertNode(set->m_hItem, item, -1, 0, TVI_LAST);
            m_owner->setModified(true);
        }
    }
    clearSelection(true);
}

int GradeList::getItemMenu(HTREEITEM item)
{
    if (item == (HTREEITEM)m_64 || getItemCell(item))
        return 0;
    return 188;
}

bool GradeList::onCommand(UINT code, HWND ctrl, UINT id)
{
    if ((code | 1) != 1 || ctrl)
        return TreeList::onCommand(code, ctrl, id);

    HTREEITEM caret = (HTREEITEM)SendMessageW(m_tree.m_hWnd, TVM_GETNEXTITEM, TVGN_CARET, 0);
    if (id == 1100)
    {
        short grade = gradeOf(caret);
        if (grade != -1)
        {
            GradeSet* from = m_sets.find(grade);
            if (from)
            {
                GetGradeDialog dialog(&m_sets);
                if (dialog.open(m_owner->m_hWnd) && dialog.doModal() == 1)
                {
                    short newGrade = dialog.m_grade;
                    if (newGrade >= 0)
                    {
                        GradeSet* to = m_sets.find(newGrade);
                        if (!to)
                        {
                            to = new GradeSet((unsigned short)newGrade);
                            m_sets.add(to);
                            wchar_t text[16];
                            swprintf_s(text, 16, L"- %d -", (int)newGrade);
                            to->m_hItem = insertItem((HTREEITEM)m_64, text, 1, 255, findInsertPos(newGrade));
                        }
                        for (HTREEITEM child = (HTREEITEM)SendMessageW(m_tree.m_hWnd, TVM_GETNEXTITEM, TVGN_CHILD, (LPARAM)caret);
                             child;
                             child = (HTREEITEM)SendMessageW(m_tree.m_hWnd, TVM_GETNEXTITEM, TVGN_CHILD, (LPARAM)caret))
                        {
                            Data::Item* item = dynamic_cast<Data::Item*>(getItemNode(child));
                            SendMessageW(m_tree.m_hWnd, TVM_DELETEITEM, 0, (LPARAM)child);
                            if (item)
                            {
                                from->detachItem(item);
                                FIELD(short, item, 0xA4) = newGrade;
                                to->add(item);
                                insertNode(to->m_hItem, item, -1, 0, TVI_LAST);
                            }
                        }
                        m_owner->setModified(true);
                    }
                }
            }
        }
        return true;
    }
    if (id != 1101)
        return TreeList::onCommand(code, 0, id);

    HTREEITEM child = (HTREEITEM)SendMessageW(m_tree.m_hWnd, TVM_GETNEXTITEM, TVGN_CHILD, (LPARAM)caret);
    if (!child)
        return true;
    Data::Item* item = dynamic_cast<Data::Item*>(getItemNode(child));
    if (!item)
        return true;
    GradeSet* set = m_sets.find(FIELD(short, item, 0xA4));
    if (!set)
        return true;
    int count = set->size();
    if (count > 2)
    {
        String message(L"Are you sure you want to clear the Grade for these ");
        message << (long)count << L" items ?";
        if (!AlertDial::confirm(m_hWnd, message.c_str()))
            return true;
    }
    SendMessageW(m_tree.m_hWnd, TVM_DELETEITEM, 0, (LPARAM)child);
    set->detachItem(item);
    FIELD(short, item, 0xA4) = 0;
    m_owner->setModified(true);
    for (child = (HTREEITEM)SendMessageW(m_tree.m_hWnd, TVM_GETNEXTITEM, TVGN_CHILD, (LPARAM)caret);
         child;
         child = (HTREEITEM)SendMessageW(m_tree.m_hWnd, TVM_GETNEXTITEM, TVGN_CHILD, (LPARAM)caret))
    {
        SendMessageW(m_tree.m_hWnd, TVM_DELETEITEM, 0, (LPARAM)child);
        set->detachItem(item);
        FIELD(short, item, 0xA4) = 0;
    }
    return true;
}

bool GradeList::onItemActivate(HTREEITEM item, WPARAM keys)
{
    Data::Item* data = dynamic_cast<Data::Item*>(getItemNode(item));
    if (data)
        return openItemEditor((Window*)m_owner, m_data, data);
    SendMessageW(m_tree.m_hWnd, TVM_EXPAND, TVE_TOGGLE, (LPARAM)item);
    return true;
}

bool GradeList::removeChildNode(Node* parent, HTREEITEM item)
{
    Data::Item* data = dynamic_cast<Data::Item*>(parent);
    if (!data)
        return false;
    FIELD(short, data, 0xA4) = 0;
    return true;
}

void GradeList::drawFirstColumn(GDC* dc, Node* node, RECT* rc)
{
    Data::Item* item = dynamic_cast<Data::Item*>(node);
    if (!item)
        return;
    ImageList* icons = reinterpret_cast<ClientData*>(reinterpret_cast<BYTE*>(g_studioFrame) + 0x230)->getImageList();
    long id = item->getId();
    int index;
    for (index = 0; index < icons->m_ids.m_capacity; ++index)
    {
        if (icons->m_ids.m_items[index] == id)
            break;
    }
    if (index >= icons->m_ids.m_capacity)
        index = -1;
    ImageList_Draw(icons->m_hList, index, dc->m_hdc, rc->left - 16, (rc->top + rc->bottom) / 2 - 8, ILD_TRANSPARENT);
    rc->left += 4;
}

HTREEITEM GradeList::findInsertPos(short grade)
{
    HTREEITEM item = (HTREEITEM)SendMessageW(m_tree.m_hWnd, TVM_GETNEXTITEM, TVGN_CHILD, (LPARAM)m_64);
    short current;
    if (item)
        goto read_grade;
    current = -1;
    while (current <= grade)
    {
        item = (HTREEITEM)SendMessageW(m_tree.m_hWnd, TVM_GETNEXTITEM, TVGN_NEXT, (LPARAM)item);
        if (!item)
            return TVI_LAST;
read_grade:
        {
            wchar_t text[32];
            memset(text, 0, sizeof(text));
            TVITEMEXW tvi;
            memset(&tvi, 0, sizeof(tvi));
            tvi.pszText = text;
            tvi.hItem = item;
            tvi.mask = TVIF_TEXT;
            tvi.cchTextMax = 32;
            if (SendMessageW(m_tree.m_hWnd, TVM_GETITEMW, 0, (LPARAM)&tvi))
                current = (text[0] == L'-') ? (short)_wtoi(&text[2]) : -1;
            else
                current = -1;
        }
    }
    return (HTREEITEM)SendMessageW(m_tree.m_hWnd, TVM_GETNEXTITEM, TVGN_PREVIOUS, (LPARAM)item);
}

short GradeList::gradeOf(HTREEITEM item)
{
    if (item)
    {
        wchar_t text[32];
        memset(text, 0, sizeof(text));
        TVITEMEXW tvi;
        memset(&tvi, 0, sizeof(tvi));
        tvi.pszText = text;
        tvi.hItem = item;
        tvi.mask = TVIF_TEXT;
        tvi.cchTextMax = 32;
        if (SendMessageW(m_tree.m_hWnd, TVM_GETITEMW, 0, (LPARAM)&tvi) && text[0] == L'-')
            return (short)_wtoi(&text[2]);
    }
    return -1;
}
