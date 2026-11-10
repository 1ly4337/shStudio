#include "p05_common.h"
#include "WorkspaceList.h"
#include "FileTree.h"

bool WorkspaceList::onItemActivate(HTREEITEM item, WPARAM )
{
    Node* node = getItemNode(item);
    if (dynamic_cast<FileTree*>(node)) {
        HWND parent = GetParent(m_hWnd);
        SendMessageW(parent, 0x406, 0, (LPARAM)node);
    } else {
        SendMessageW(m_tree.m_hWnd, TVM_EXPAND, TVE_TOGGLE, (LPARAM)item);
    }
    return true;
}
