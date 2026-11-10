#include <windows.h>
#include <commctrl.h>
#include <cwchar>
#include "WorkspaceDataSet.h"
#include "TreeList.h"
#include "FolderRoot.h"
#include "DataProvider.h"
#include "ContentProvider.h"
#include "ArchiveProvider.h"
#include "DataFileProvider.h"
#include "FolderTreeFill.h"
#include "String.hpp"
#include "ida_compat.h"
#include "_imports_p09.h"
#include "_imports_g4.h"

WorkspaceDataSet::WorkspaceDataSet(TreeList* tree)
    : WorkspaceNode((void*)FIELD(HWND, tree, 0x14), 0, 132)
{
    m_tree = tree;
    m_source = 0;
}

void WorkspaceDataSet::unload()
{
    m_tree->deleteChildren((HTREEITEM)(INT_PTR)m_data);
    m_root.reset();
    g_contentProvider = 0;
    if (m_source)
    {
        delete m_source;
        m_source = 0;
    }
}

bool WorkspaceDataSet::open(const String& path, int format)
{
    g_contentProvider = 0;
    if (m_source)
        delete m_source;
    unsigned length = path.m_length;
    if (length >= 4 && !_wcsnicmp(path.m_buffer + length - 4, L".saf", 4))
    {
        m_source = new ArchiveProvider(&m_root, format);
    }
    else if (path.m_length && path.m_buffer[path.m_length - 1] == L'\\')
    {
        m_source = new DataFileProvider(&m_root, format);
    }
    g_contentProvider = m_source;
    FIELD(String, &m_root, 0x0C) = path;
    if (m_source->open(g_hMainWnd, &m_root))
        return false;
    refresh(m_tree);
    return true;
}

void WorkspaceDataSet::refresh(TreeList* tree)
{
    SetCursor(hCursor);
    tree->deleteChildren((HTREEITEM)(INT_PTR)m_data);
    m_root.m_data = m_data;
    fillFolderTree(tree, &m_root);
    SendMessageW(FIELD(HWND, tree, 0x38), TVM_EXPAND, TVE_EXPAND, m_data);
    SetCursor(g_arrowCursor);
    RECT rc;
    LPRECT itemRect = tree->m_tree.getItemRect(&rc, (HTREEITEM)(INT_PTR)m_data);
    InvalidateRect(FIELD(HWND, tree, 0x38), itemRect, FALSE);
}
