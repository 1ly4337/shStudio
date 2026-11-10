#include "p05_common.h"
#include "ServerNode.h"
#include "FileDialogs.h"
#include "FolderOption.h"
#include "menus.h"
#include "FolderTreeFill.h"
#include "Popup.h"
#include "TreeList.h"
#include "TreeList.h"

#define SERVER_ROOT_PATH(self) FIELD(String, self, 0x44)

ServerNode::ServerNode(void* owner, void* )
    : WorkspaceNode(owner, 16, 133), m_name(), m_root()
{
    m_name.append(L"Server", -1);
    m_connected = false;
}

ServerNode::~ServerNode()
{
}

const wchar_t* ServerNode::getString(DWORD tag) const
{
    if (tag == 'name')
        return m_name.m_buffer;
    if (tag != 'stat')
        return 0;
    const wchar_t* result = L"Connected";
    if (!m_connected)
        return L"";
    return result;
}

void ServerNode::onCommand(int cmd, int arg)
{
    TreeList* tree = (TreeList*)arg;
    if (cmd == 0xE150) {
        String location = g_preferences.getString(L"Server Data", L"location", 0);
        if (FileDialogs_selectFolder(tree->m_hWnd, &location, L"Select the server files repository",
                                     0, true, FolderOption::isClientFolder)) {
            if (!location.endsWithNoCase(L"PSM_Client\\"))
                location << L"PSM_Client\\";
            connect(tree, &location);
            SendMessageW(tree->m_tree.m_hWnd, TVM_EXPAND, TVE_EXPAND, FIELD(LPARAM, this, 0x08));
            g_preferences.writeString(L"Server Data", L"location", location.m_buffer);
            g_preferences.writeInt(L"Server Data", L"Connect", 1);
        }
        tree->invalidate(true);
    } else if (cmd == 0xE151) {
        m_name.assign(L"Server");
        m_root.reset();
        String& path = SERVER_ROOT_PATH(this);
        void* buf = path.m_buffer;
        path.m_length = 0;
        path.m_capacity = 0;
        if (buf) {
            delete[] (wchar_t*)buf;
            path.m_buffer = 0;
        }
        m_connected = false;
        g_preferences.writeInt(L"Server Data", L"Connect", 0);
        tree->deleteChildren(FIELD(HTREEITEM, this, 0x08));
        tree->invalidate(true);
    }
}

void ServerNode::onInitItemMenu(HMENU menu, HWND , HTREEITEM )
{
    setMenuItemState(menu, 0xE151, SERVER_ROOT_PATH(this).m_length != 0);
}

int ServerNode::connect(TreeList* tree, String* path)
{
    m_connected = false;
    String& rootPath = SERVER_ROOT_PATH(this);
    if (path != &rootPath) {
        int len = path->m_length;
        const wchar_t* src = path->m_buffer;
        void* buf = rootPath.m_buffer;
        rootPath.m_length = 0;
        rootPath.m_capacity = 0;
        if (buf) {
            delete[] (wchar_t*)buf;
            rootPath.m_buffer = 0;
        }
        rootPath.append(src, len);
    }
    tree->deleteChildren(FIELD(HTREEITEM, this, 0x08));
    int result = populate(tree);
    if (!result) {
        m_name.assign(L"Server - ");
        m_name.append(path->m_buffer, path->m_length);
        tree->invalidateItem(FIELD(int, this, 0x08));
        result = 0;
        m_connected = true;
    }
    return result;
}

int ServerNode::populate(TreeList* tree)
{
    SERVER_ROOT_PATH(this) << L"Bin\\Data\\";
    int err = (int)m_root.rescan(0);
    if (err) {
        ((Popup*)g_studioFrame)->setStatusText(L"The selected folder is invalid.", 0);
        return err;
    }
    LPARAM parent = FIELD(LPARAM, this, 0x08);
    LPARAM after = (LPARAM)TVI_LAST;
    if (parent == (LPARAM)TVI_ROOT)
        after = (LPARAM)TVI_ROOT;
    LPARAM ins[16];
    memset(ins, 0, sizeof(ins));
    ins[0] = parent;
    ins[1] = after;
    ins[6] = (LPARAM)L"PSM_Client\\Bin\\Data";
    ins[8] = 1;
    ins[9] = 1;
    ins[2] = 99;
    ins[10] = 1;
    FIELD(LRESULT, this, 0x40) = SendMessageW(tree->m_tree.m_hWnd, TVM_INSERTITEMW, 0, (LPARAM)ins);
    fillFolderTree(tree, &m_root);
    return 0;
}
