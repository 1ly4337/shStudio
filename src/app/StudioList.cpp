#include "p05_common.h"
#include "StudioList.h"
#include "StudioFrame.h"
#include "ClientNode.h"
#include "ServerNode.h"
#include "SqlSvrNode.h"
#include "AutoPtrCell.h"
#include "FileTree.h"
#include "FolderTree.h"
#include "WorkspaceItemNode.h"

static LRESULT studioInsertRoot(HWND hTree, void* cell, int image)
{
    LPARAM ins[16];
    memset(ins, 0, 0x40);
    ins[0] = (LPARAM)TVI_ROOT;
    ins[1] = (LPARAM)TVI_ROOT;
    ins[11] = (LPARAM)cell;
    ins[6] = -1;
    ins[2] = 39;
    ins[8] = image;
    ins[9] = image;
    return SendMessageW(hTree, TVM_INSERTITEMW, 0, (LPARAM)ins);
}

bool StudioList::onCreate()
{
    bool result = WorkspaceList::onCreate();
    if (!result)
        return result;

    HTREEITEM clientItem;
    ClientNode* client = m_client;
    if (client) {
        Cell* cell = new Cell(client);
        clientItem = insertCell(cell, TVI_ROOT, 0, 0, TVI_ROOT);
    } else {
        clientItem = 0;
    }
    m_clientItem = clientItem;

    AutoPtrCell* serverCell = new AutoPtrCell(new ServerNode(FIELD(void*, m_frame, 0x14), m_frame));
    LRESULT serverItem;
    if (serverCell) {
        serverItem = studioInsertRoot(m_tree.m_hWnd, serverCell, 16);
        FIELD(LRESULT, serverCell->m_node, 0x08) = serverItem;
    } else {
        serverItem = 0;
    }
    m_serverItem = (HTREEITEM)serverItem;

    AutoPtrCell* sqlCell = new AutoPtrCell(new SqlSvrNode(FIELD(void*, m_frame, 0x14))   );
    LRESULT sqlItem;
    if (sqlCell) {
        sqlItem = studioInsertRoot(m_tree.m_hWnd, sqlCell, 17);
        FIELD(LRESULT, sqlCell->m_node, 0x08) = sqlItem;
    } else {
        sqlItem = 0;
    }
    m_sqlItem = (HTREEITEM)sqlItem;
    return true;
}

bool StudioList::onCommand(UINT code, HWND ctrl, UINT id)
{
    if ((code | 1) != 1 || ctrl)
        return TreeList::onCommand(code, ctrl, id);
    if ((unsigned int)(id - 0xE150) > 0x1F) {
        if ((unsigned int)(id - 0xE100) <= 0x1F) {
            m_frame->onCommand(code, 0, id);
            return true;
        }
        return TreeList::onCommand(code, ctrl, id);
    }
    HTREEITEM sel = (HTREEITEM)SendMessageW(m_tree.m_hWnd, TVM_GETNEXTITEM, TVGN_CARET, 0);
    Node* node = getItemNode(sel);
    WorkspaceNode* ws = dynamic_cast<WorkspaceNode*>(node);
    if (ws)
        ws->onCommand(id, (int)this);
    return true;
}

int StudioList::getNodeImage(Node* node)
{
    if (node)
        return node->getId();
    return 0;
}

int StudioList::getItemMenu(HTREEITEM item)
{
    Node* node = getItemNode(item);
    WorkspaceNode* ws = dynamic_cast<WorkspaceNode*>(node);
    if (ws)
        return ws->getPopupMenu();
    String text = getItemText(item);
    (void)(text == L"world");
    return 0;
}

void StudioList::onInitItemMenu(HMENU menu, HWND tree, HTREEITEM item)
{
    Node* node = getItemNode(item);
    WorkspaceNode* ws = dynamic_cast<WorkspaceNode*>(node);
    if (ws)
        ws->onInitItemMenu(menu, tree, item);
}

bool StudioList::onItemActivate(HTREEITEM item, WPARAM keys)
{
    Node* node = getItemNode(item);
    if (FileTree* file = dynamic_cast<FileTree*>(node)) {
        m_frame->reopenDocument(file);
        return true;
    }
    if (dynamic_cast<FolderTree*>(node)) {
        SendMessageW(m_tree.m_hWnd, TVM_EXPAND, TVE_TOGGLE, (LPARAM)item);
        return true;
    }
    WorkspaceItemNode* w = dynamic_cast<WorkspaceItemNode*>(node);
    if (w) {
        w->activate((int)(INT_PTR)this);
        return true;
    }
    return TreeList::onItemActivate(item, keys);
}

bool StudioList::onCtrlDropFile(int , POINT* , const wchar_t* src)
{
    if (!src)
        return false;
    String path;
    path.append(src, -1);
    bool inside;
    {
        String current;
        String* cur = m_frame->getWorkspacePath(&current);
        const wchar_t* curBuf = cur->m_buffer;
        if (!curBuf) {
            inside = false;
        } else {
            unsigned int n = (unsigned int)wcslen(curBuf);
            if (n > (unsigned int)path.m_length)
                inside = false;
            else
                inside = wcsncmp(path.m_buffer, curBuf, n) == 0;
        }
    }
    bool flag = inside;
    if (inside)
        flag = m_frame->findFile(src) != 0;
    return m_frame->openDocument(0, path.m_buffer, flag);
}

bool StudioList::onNodeCommand(UINT cmd, HTREEITEM item)
{
    Node* node = getItemNode(item);
    WorkspaceNode* ws = dynamic_cast<WorkspaceNode*>(node);
    if (!ws)
        return false;
    ws->onCommand(cmd, (int)this);
    return true;
}
