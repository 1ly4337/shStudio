#include "TreeList.h"
#include <commctrl.h>

bool TreeList::setFirstVisible(LPARAM item)
{
    return SendMessageW(m_tree.m_hWnd, TVM_SELECTITEM, TVGN_FIRSTVISIBLE, item) != 0;
}
