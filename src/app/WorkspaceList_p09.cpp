#include <windows.h>
#include <commctrl.h>
#include <cstring>
#include "WorkspaceList.h"
#include "HDropManager.h"
#include "ImageList.h"
#include "Popup.h"
#include "ida_compat.h"
#include "_imports_p09.h"
#include "_imports_g5.h"

#pragma warning(disable : 4066 4498)

bool WorkspaceList::onCreate()
{
    bool result = TreeList::onCreate();
    if (!result)
        return result;

    int bitmapId = m_popupMenu;
    ImageList images;
    images.m_cx = 16;
    images.load((WORD)bitmapId, 0);
    HIMAGELIST handle = images.m_hList;
    images.m_hList = 0;
    SendMessageW(FIELD(HWND, this, 0x38), TVM_SETIMAGELIST, TVSIL_NORMAL, (LPARAM)handle);
    SendMessageW(FIELD(HWND, this, 0x38), TVM_SETIMAGELIST, TVSIL_STATE, (LPARAM)handle);

    LVCOLUMNW column;
    memset(&column, 0, 0x2C);
    column.mask = 7;
    column.fmt = 0;
    column.cx = 240;
    column.pszText = (LPWSTR)L"Filename";
    this->TreeList::insertColumn(&column, 'name');

    int format = 17;
    memset(&column, 0, 0x2C);
    if (!FIELD(int, this, 0x84))
        format = 16;
    column.mask = 7;
    column.fmt = format;
    column.cx = 60;
    column.pszText = (LPWSTR)L"Size";
    this->TreeList::insertColumn(&column, 'size');

    format = 2;
    memset(&column, 0, 0x2C);
    column.mask = 7;
    if (!FIELD(int, this, 0x84))
        format = 0;
    column.fmt = format;
    column.cx = 100;
    column.pszText = (LPWSTR)L"Date";
    this->TreeList::insertColumn(&column, 'time');

    format = 2;
    memset(&column, 0, 0x2C);
    if (!FIELD(int, this, 0x84))
        format = 0;
    column.mask = 7;
    column.fmt = format;
    column.cx = 100;
    column.pszText = (LPWSTR)L"Status";
    this->TreeList::insertColumn(&column, 'stat');

    setFont((HFONT)getStdFont(0));
    FIELD(HDropManager*, this, 0x80) = new HDropManager(this, (char*)this + 0x34);
    return true;
}

void WorkspaceList::reportDropError(int error)
{
    HWND parent = GetParent(m_hWnd);
    Popup* popup = dynamic_cast<Popup*>(Window::fromHwnd(parent, true));
    if (!popup)
        return;
    if (error == 3)
        popup->setStatusText(L"A dropped file can not be accessed. Make sure pathname is valid", 0);
    else if (error == 183)
        popup->setStatusText(L"A dropped file already exists in this list.", 0);
}
