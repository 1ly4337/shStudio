#include "ListCtrlOD.h"
#include "Node.h"
#include "Window.h"
#include "Clipboard.h"
#include "AlertDial.h"
#include "menus.h"
#include "iStream.hpp"
#include "oStream.hpp"
#include "ida_compat.h"
#include "_imports_p09.h"
#include "_imports_g7.h"

int ListCtrlOD::insertItem(Node* , int )
{
    throw "must be overridden";
}

void ListCtrlOD::updateItem(int index)
{
    if (index == -2)
        index = getNextSelected(0);
    if (index >= 0)
    {
        RECT rc;
        InvalidateRect(m_hWnd, getItemRect(&rc, index, 0), TRUE);
    }
}

void ListCtrlOD::updateAll()
{
    int count = getCount(0);
    SendMessageW(m_hWnd, LVM_REDRAWITEMS, 0, count - 1);
    UpdateWindow(m_hWnd);
}

Node* ListCtrlOD::getItemParam(int index)
{
    return getNode(index);
}

bool ListCtrlOD::onCommand(int code, LPARAM ctrl, int id)
{
    if ((code | 1) != 1 || ctrl)
        return false;
    switch (id)
    {
    case 0xE122:
        cut();
        return true;
    case 0xE123:
        copy();
        return true;
    case 0xE124:
        paste();
        return true;
    case 0xE12A:
        insertNew(-1);
        return true;
    case 0xE12B:
        deleteSelected(L"to delete");
        return true;
    case 0xE12C:
        clearSelected();
        return true;
    case 0xE125:
        selectAll();
        return true;
    case 0xE129:
    case 0xE128:
    {
        Window* window = Window::fromHwnd(m_hWnd, true);
        if (!window)
            return true;
        PostMessageW(window->getHwnd(), WM_COMMAND, (unsigned short)id | ((unsigned short)code << 16), 0);
        return true;
    }
    default:
        return ListCtrl::onCommand(code, 0, id);
    }
}

bool ListCtrlOD::onInitMenuPopup(HMENU menu)
{
    BOOL hasSelection = getCount(1) > 0;
    char multiSelect = ~(unsigned char)((unsigned int)GetWindowLongW(m_hWnd, GWL_STYLE) >> 2);
    setMenuItemState(menu, 0xE122, hasSelection);
    setMenuItemState(menu, 0xE123, hasSelection);
    setMenuItemState(menu, 0xE12B, hasSelection);
    setMenuItemState(menu, 0xE12C, hasSelection);
    setMenuItemState(menu, 0xE125, (multiSelect & 1) != 0);
    UINT format = getClipboardFormat();
    int canPaste = 0;
    if ((int)format > 512)
    {
        bool found = false;
        if (OpenClipboard(g_clipboard.m_hOwner))
        {
            UINT current = EnumClipboardFormats(0);
            if (current)
            {
                while (current != format)
                {
                    current = EnumClipboardFormats(current);
                    if (!current)
                        goto done;
                }
                found = true;
            }
        done:
            CloseClipboard();
            if (found)
                canPaste = 1;
        }
    }
    setMenuItemState(menu, 0xE124, canPaste);
    return true;
}

bool ListCtrlOD::onKey(short vk, int flags, bool down)
{
    if (!down)
        return false;
    if (GetKeyState(VK_CONTROL) < 0)
    {
        if (vk == VK_CONTROL)
            return false;
        if (vk == 'W')
        {
            HWND parent = GetParent(m_hWnd);
            PostMessageW(parent, WM_COMMAND, 0xE102, 0);
            return true;
        }
        if (vk == 'D')
        {
            selectItem(-1);
            return true;
        }
        if (vk == 'V')
        {
            paste();
            return true;
        }
        int selected = getCount(1);
        if (selected <= 0)
            return false;
        switch (vk)
        {
        case 'X':
            cut();
            return true;
        case 'C':
        case VK_INSERT:
            copy();
            return true;
        case VK_UP:
            if (selected == 1)
            {
                int index = getNextSelected(0);
                if (index <= 0 || !moveItemUp(index))
                    return true;
                setItemState(index, 0, LVIS_SELECTED);
                setItemState(index - 1, LVIS_SELECTED, 0);
                UpdateWindow(m_hWnd);
                return true;
            }
            break;
        default:
            if (vk == VK_DOWN && selected == 1)
            {
                int index = getNextSelected(0);
                if (index >= getCount(0) - 1 || !moveItemDown(index))
                    return true;
                setItemState(index, 0, LVIS_SELECTED);
                setItemState(index + 1, LVIS_SELECTED, 0);
                UpdateWindow(m_hWnd);
                return true;
            }
            break;
        }
        return ListCtrl::onKey(vk, flags, down);
    }
    if (GetKeyState(VK_SHIFT) < 0)
    {
        if (vk == VK_INSERT)
        {
            paste();
            return ListCtrl::onKey(VK_INSERT, flags, down);
        }
        if (vk == VK_DELETE)
            clearSelected();
        return ListCtrl::onKey(vk, flags, down);
    }
    if (vk != VK_INSERT)
        return ListCtrl::onKey(vk, flags, down);
    insertNew(getNextSelected(0));
    return true;
}

Node* ListCtrlOD::readNode(iStream& in)
{
    Node* node = createNode();
    if (node)
        static_cast<Streamable*>(node)->read(in, 'clip');
    return node;
}

bool ListCtrlOD::cut()
{
    bool result = copy();
    if (result)
        return deleteSelected(L"to cut");
    return result;
}

bool ListCtrlOD::copy()
{
    int selected = (int)SendMessageW(m_hWnd, LVM_GETSELECTEDCOUNT, 0, 0);
    if (selected < 1 || (unsigned int)(getClipboardFormat() - 1) > 0x2FE)
        return false;
    oStream out(false, 1200);
    String csv;
    String sql;
    Node* node = 0;
    bool result;
    if (selected == 1)
    {
        int index = getNextSelected(0);
        if (index == -1)
            return false;
        node = getNode(index);
        if (!node)
            return false;
        out << (unsigned long)1;
        static_cast<Streamable*>(node)->write(out, 'clip');
        static_cast<Streamable*>(node)->toText(csv, 'csv ');
        static_cast<Streamable*>(node)->toText(sql, 'sql ');
    }
    else
    {
        out << (unsigned long)selected;
        int copied = 0;
        int count = getCount(0);
        for (int i = 0; i < count; ++i)
        {
            int done;
            if (SendMessageW(m_hWnd, LVM_GETITEMSTATE, i, LVIS_SELECTED) == LVIS_SELECTED)
            {
                node = getNode(i);
                if (!node)
                    return false;
                static_cast<Streamable*>(node)->write(out, 'clip');
                if (static_cast<Streamable*>(node)->toText(csv, 'csv '))
                    csv << L"\r\n";
                if (static_cast<Streamable*>(node)->toText(sql, 'sql '))
                    sql << L"\r\n";
                done = ++copied;
            }
            else
            {
                done = copied;
            }
            if (done == selected)
                break;
        }
    }
    UINT format = getClipboardFormat();
    if (!Clipboard::setData(format, out, true))
        return false;
    if (selected == 1)
        onCopied(node);
    unsigned length = sql.length();
    if (length)
    {
        if (csv.length())
        {
            csv << L"\r\n";
            length = sql.length();
        }
        csv.append(sql.c_str(), length);
    }
    if (csv.length())
        result = Clipboard::setText(csv.c_str(), false);
    else
        result = true;
    return result;
}

bool ListCtrlOD::onCopied(Node* node)
{
    long id = node->getId();
    if (id < 0)
        return true;
    int image;
    int i = 0;
    if (m_imageMap.m_count <= 0)
    {
        image = 0;
    }
    else
    {
        while (m_imageMap.m_table[i].key != id)
        {
            if (++i >= m_imageMap.m_count)
                break;
        }
        if (i >= m_imageMap.m_count || i == -1)
        {
            image = 0;
        }
        else
        {
            image = m_imageMap.m_table[i].value;
            if (image < 0)
                return true;
        }
    }
    HIMAGELIST list = (HIMAGELIST)SendMessageW(m_hWnd, LVM_GETIMAGELIST, LVSIL_NORMAL, 0);
    if (list || (list = (HIMAGELIST)SendMessageW(m_hWnd, LVM_GETIMAGELIST, LVSIL_SMALL, 0)) != 0)
    {
        HBITMAP bitmap = imageListBitmap(list, image);
        if (bitmap)
        {
            clipboard_putBitmap(bitmap);
            DeleteObject(bitmap);
        }
    }
    return true;
}

bool ListCtrlOD::paste()
{
    iStream in(false, 1200);
    UINT format = getClipboardFormat();
    if (!Clipboard::getData(format, in))
        return false;
    int updated = 0;
    int added = 0;
    int total = (int)in.readInt();
    if (total >= 1)
    {
        setRedraw(false);
        for (int i = 0; i < total; ++i)
        {
            Node* pasted = readNode(in);
            if (!pasted)
                break;
            Node* target;
            if (total == 1 && SendMessageW(m_hWnd, LVM_GETSELECTEDCOUNT, 0, 0) == 1)
            {
                target = getNode(getNextSelected(0));
            }
            else
            {
                selectItem(-1);
                UpdateWindow(m_hWnd);
                target = findNode(pasted);
            }
            if (target)
            {
                int index = indexOfNode(target);
                if (index < 0)
                    return false;
                if (static_cast<Streamable*>(target)->copyFrom(static_cast<Streamable*>(pasted)))
                {
                    ++updated;
                    if (total == 1)
                        onPasted(target);
                    RECT rc;
                    InvalidateRect(m_hWnd, getItemRect(&rc, index, 0), TRUE);
                }
                else
                {
                    MessageBeep(MB_ICONEXCLAMATION);
                }
                delete pasted;
            }
            else if (insertItem(pasted, -1) >= 0)
            {
                ++added;
                if (total == 1)
                    onPasted(pasted);
            }
            else
            {
                MessageBeep(MB_ICONEXCLAMATION);
                delete pasted;
            }
        }
        setRedraw(true);
        if (added)
        {
            if (added > 0)
                setItemCount(added + getCount(0));
        }
        else if (!updated)
        {
            return false;
        }
        onPasteDone(added > 0);
        onChanged();
    }
    return true;
}

bool ListCtrlOD::selectAll()
{
    if (!IsWindow(m_hWnd) || (GetWindowLongW(m_hWnd, GWL_STYLE) & LVS_SINGLESEL) != 0)
        return false;
    int count = getCount(0);
    setRedraw(false);
    for (int i = 0; i < count; ++i)
    {
        LVITEMW item;
        memset(&item, 0, sizeof(item));
        item.stateMask = LVIS_SELECTED;
        item.state = LVIS_SELECTED;
        SendMessageW(m_hWnd, LVM_SETITEMSTATE, i, (LPARAM)&item);
    }
    setRedraw(true);
    return true;
}

bool ListCtrlOD::insertNew(int index)
{
    int count = getCount(0);
    Node* node = createNode();
    if (!node)
        return false;
    int inserted = insertItem(node, index);
    if (inserted >= 0)
    {
        setItemCount(count + 1);
        selectItem(inserted);
        SendMessageW(m_hWnd, LVM_ENSUREVISIBLE, inserted, 0);
        focusItem(inserted, true);
        onChanged();
        return true;
    }
    delete node;
    return false;
}

bool ListCtrlOD::deleteSelected(const wchar_t* verb)
{
    int count = getCount(0);
    int selected = getCount(1);
    if (selected < 1)
        return false;
    if (selected == 1)
    {
        int index = getNextSelected(0);
        if (index != -1 && confirm(verb, 1) && removeItem(index))
        {
            setItemCount(count - 1);
            onChanged();
            selectItem(index);
            focusItem(index, false);
            return true;
        }
        return false;
    }
    if (!confirm(verb, selected))
        return false;
    setRedraw(false);
    int removed = 0;
    for (int i = count - 1; i >= 0; --i)
    {
        if (SendMessageW(m_hWnd, LVM_GETITEMSTATE, i, LVIS_SELECTED) == LVIS_SELECTED)
        {
            if (!removeItem(i))
                break;
            ++removed;
            if (!--selected)
                break;
        }
    }
    selectItem(-1);
    setItemCount(count - removed);
    setRedraw(true);
    onChanged();
    return true;
}

bool ListCtrlOD::confirm(const wchar_t* verb, int count)
{
    String message;
    message << L"Are you sure you want ";
    message << verb;
    String* tail = &message;
    const wchar_t* end;
    if (count == 1)
    {
        end = L" this item ?";
    }
    else
    {
        message << L" these ";
        tail = &(message << (unsigned long)count);
        end = L" items ?";
    }
    *tail << end;
    return AlertDial::alert(m_hWnd, message.c_str(), 0, MB_YESNO | MB_ICONQUESTION) == IDYES;
}

bool ListCtrlOD::clearSelected()
{
    int selected = getCount(1);
    if (selected < 1)
        return false;
    if (selected == 1)
    {
        int index = getNextSelected(0);
        if (index == -1)
            return false;
        bool ok = clearItem(index);
        if (ok)
        {
            updateItem(index);
            onChanged();
        }
        return ok;
    }
    String message;
    (message << L"Are you sure you want to clear these ") << (long)selected << L" items ?";
    bool result;
    if (AlertDial::alert(m_hWnd, message.c_str(), 0, MB_YESNO | MB_ICONQUESTION) == IDYES)
    {
        setRedraw(false);
        int cleared = 0;
        for (int i = getCount(0) - 1; i >= 0; --i)
        {
            if (SendMessageW(m_hWnd, LVM_GETITEMSTATE, i, LVIS_SELECTED) == LVIS_SELECTED)
            {
                if (!clearItem(i))
                    break;
                if (++cleared == selected)
                    break;
            }
        }
        setRedraw(true);
        if (cleared)
        {
            onChanged();
            result = true;
        }
        else
        {
            result = false;
        }
    }
    else
    {
        result = false;
    }
    return result;
}
