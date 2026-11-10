#include "p05_common.h"
#include "ListCtrl.h"
#include "Node.h"

int ListCtrl::getCount(int selectedOnly)
{
    UINT msg = LVM_GETITEMCOUNT;
    if (selectedOnly == 1)
        msg = LVM_GETSELECTEDCOUNT;
    return (int)SendMessageW(m_hWnd, msg, 0, 0);
}

LRESULT ListCtrl::setItemCount(int count)
{
    return SendMessageW(m_hWnd, LVM_SETITEMCOUNT, count, 0);
}

bool ListCtrl::deleteItem(int index)
{
    return SendMessageW(m_hWnd, LVM_DELETEITEM, index, 0) == 1;
}

const wchar_t* ListCtrl::getItemText(Node* node, int columnId)
{
    if (node)
        return node->getString(columnId);
    return L"";
}

const wchar_t* ListCtrl::getClassName()
{
    return L"SysListView32";
}

DWORD ListCtrl::getStyle()
{
    return 0x10000100;
}

DWORD ListCtrl::getExStyle()
{
    return 32;
}
