#include "ClientNode.h"
#include "ContentProvider.h"
#include "DataFileProvider.h"
#include "ArchiveProvider.h"
#include "ExpandArchiveDialog.h"
#include "ArchiveExtractor.h"
#include "FolderTreeFill.h"
#include "FolderRoot.h"
#include "TreeList.h"
#include "ToggleNode.h"
#include "Cell.h"
#include "Time.hpp"
#include "Preferences.h"
#include "ContentFiles.h"
#include "ChooseCodepageDialog.h"
#include "ida_compat.h"
#include "_imports_p09.h"
#include "_imports_g2.h"
#include <commctrl.h>

static inline ContentProvider*& sourceOf(ClientNode* node) { return FIELD(ContentProvider*, node, 0x18); }
static inline TreeList* treeOf(ClientNode* node)           { return FIELD(TreeList*, node, 0x1C); }
static inline FolderRoot* rootOf(ClientNode* node)         { return (FolderRoot*)((uint8_t*)node + 0x20); }
static inline String& rootPath(FolderRoot* root)           { return FIELD(String, root, 0x0C); }
static inline HWND treeHwnd(TreeList* tree)                { return FIELD(HWND, tree, 0x38); }
static inline wchar_t* fileName(FileTree* file)            { return FIELD(wchar_t*, file, 0x34); }

ClientNode::ClientNode(TreeList* tree, int )
    : WorkspaceDataSet(tree),
      m_name(),
      m_clientData(tree, rootOf(this))
{
    m_name.append(L"Client", (unsigned)-1);
}

static void addShortcut(TreeList* tree, FolderRoot* root, const wchar_t* path, HTREEITEM parent)
{
    FileTree* file = (FileTree*)root->findFile(path);
    if (file)
    {
        int image = getContentFileType(fileName(file));
        Cell* cell = new Cell((Node*)file);
        tree->insertCell(cell, parent, image, 0, TVI_LAST);
    }
}

void ClientNode::refresh(TreeList* tree)
{
    SetCursor(hCursor);
    tree->deleteChildren((HTREEITEM)(INT_PTR)m_data);
    FolderRoot* root = rootOf(this);
    bool binary = root->FolderTree::findChildFolder(L"BinarySData") != 0;
    ToggleNode* shortcuts = new ToggleNode(binary ? L"Shortcuts (5.x)" : L"Shortcuts");
    HTREEITEM parent = (HTREEITEM)(INT_PTR)m_data;
    HTREEITEM shortcutsItem;
    if (shortcuts)
        shortcutsItem = tree->insertCell(new Cell(shortcuts), parent, 1, 1, TVI_LAST);
    else
        shortcutsItem = 0;
    addShortcut(tree, root, L"Cash.sdata", shortcutsItem);
    addShortcut(tree, root, L"Character\\Skill.sdata", shortcutsItem);
    addShortcut(tree, root, L"Item\\Item.sdata", shortcutsItem);
    addShortcut(tree, root, L"Item\\DualLayerClothes.sdata", shortcutsItem);
    addShortcut(tree, root, L"Item\\SetItem.sdata", shortcutsItem);
    addShortcut(tree, root, L"Monster\\Monster.mon", shortcutsItem);
    addShortcut(tree, root, L"Monster\\Monster.sdata", shortcutsItem);
    addShortcut(tree, root, L"Monster\\NpcSkill.sdata", shortcutsItem);
    addShortcut(tree, root, L"Npc\\npc.mon", shortcutsItem);
    addShortcut(tree, root, L"Npc\\NpcQuest.sdata", shortcutsItem);
    if (binary)
    {
        ToggleNode* shortcuts6 = new ToggleNode(L"Shortcuts (6.x)");
        HTREEITEM parent6 = (HTREEITEM)(INT_PTR)m_data;
        HTREEITEM shortcuts6Item;
        if (shortcuts6)
            shortcuts6Item = tree->insertCell(new Cell(shortcuts6), parent6, 1, 1, TVI_LAST);
        else
            shortcuts6Item = 0;
        addShortcut(tree, root, L"BinarySData\\DBItemSellData.sdata", shortcuts6Item);
        addShortcut(tree, root, L"BinarySData\\DBSkillData.sdata", shortcuts6Item);
        addShortcut(tree, root, L"BinarySData\\DBItemData.SData", shortcuts6Item);
        addShortcut(tree, root, L"BinarySData\\DBDualLayerClothesData.sdata", shortcuts6Item);
        addShortcut(tree, root, L"BinarySData\\DBSetItemData.sdata", shortcuts6Item);
        addShortcut(tree, root, L"BinarySData\\DBMonsterData.sdata", shortcuts6Item);
        addShortcut(tree, root, L"BinarySData\\DBNpcSkillData.sdata", shortcuts6Item);
    }
    ToggleNode* allFiles = new ToggleNode(L"All Files");
    HTREEITEM parentAll = (HTREEITEM)(INT_PTR)m_data;
    HTREEITEM allFilesItem = 0;
    if (allFiles)
        allFilesItem = tree->insertCell(new Cell(allFiles), parentAll, 1, 0, TVI_LAST);
    {
        String folder = getContentFolder();
        if (&folder != &rootPath(root))
            rootPath(root) = folder;
    }
    FIELD(HTREEITEM, root, 0x08) = allFilesItem;
    fillFolderTree(tree, (FolderTree*)root);
    SendMessageW(treeHwnd(tree), TVM_EXPAND, TVE_EXPAND, (LPARAM)m_data);
    SetCursor(g_arrowCursor);
    m_name = L"Client - ";
    {
        String directory = getContentDirectory();
        m_name.append(directory.m_buffer, directory.m_length);
    }
    tree->invalidateItem(m_data);
}

void ClientNode::unload()
{
    m_name = L"Client";
    m_clientData.clear();
    treeOf(this)->deleteChildren((HTREEITEM)(INT_PTR)m_data);
    rootOf(this)->reset();
    ContentProvider* source = sourceOf(this);
    g_contentProvider = 0;
    if (source)
    {
        delete source;
        sourceOf(this) = 0;
    }
}

bool ClientNode::createWorkspace()
{
    ContentProvider* source = sourceOf(this);
    if (source)
    {
        delete source;
        sourceOf(this) = 0;
    }
    bool result;
    String file = g_preferences.getString(L"Workspace", L"file", 0);
    if (FileDialogs_open(0, &file, L"Archive Files (*.saf)", L"Select a Shaiya data file", 1))
    {
        g_preferences.writeString(L"Workspace", L"file", file.m_buffer);
        String workspace = g_preferences.getString(L"Workspace", L"last", 0);
        if (FileDialogs_selectFolder(treeOf(this)->m_hWnd, &workspace, L"Select or create a workspace", 0, 0, 0))
        {
            g_preferences.writeString(L"Workspace", L"last", workspace.m_buffer);
            ArchiveExtractor extractor;
            DWORD error = extractor.open(file.m_buffer);
            if (error)
            {
                const wchar_t* text;
                if (error == 13)
                {
                    text = L"The .sah indexes file is invalid.";
                }
                else
                {
                    text = L"The archive file is locked.\nMake sure the client game is not running.";
                    if (error != 32)
                        text = L"An error has occured.\nThe archive can not be opened.";
                }
                AlertDial::alert(treeOf(this)->m_hWnd, text, 0, 16);
                result = false;
            }
            else
            {
                ExpandArchiveDialog dialog(&extractor, rootOf(this), workspace.m_buffer);
                if (dialog.create(treeOf(this)->m_hWnd))
                {
                    if (dialog.doModal() == 1)
                    {
                        Time now;
                        {
                            String stamp = now.toAsn1();
                            g_preferences.writeString(L"Workspace", workspace.m_buffer, stamp.m_buffer);
                        }
                        int format = ChooseCodepageDialog::run(g_hMainWnd, workspace.m_buffer);
                        DataFileProvider* provider = new DataFileProvider(rootOf(this), format);
                        provider->m_directory = workspace.m_buffer;
                        g_contentProvider = provider;
                        sourceOf(this) = provider;
                    }
                    else
                    {
                        rootOf(this)->reset();
                        m_name = L"Client";
                    }
                }
                refresh(treeOf(this));
                result = true;
            }
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

bool ClientNode::open(const String& path, int format)
{
    unsigned length = path.m_length;
    ContentProvider* provider;
    if (length < 4 || _wcsnicmp(path.m_buffer + length - 4, L".saf", 4))
        provider = new DataFileProvider(rootOf(this), format);
    else
        provider = new ArchiveProvider(rootOf(this), format);
    sourceOf(this) = provider;
    if (&path != &rootPath(rootOf(this)))
        rootPath(rootOf(this)) = path;
    FIELD(int, this, 0xA8) = format;
    DataProvider* data = (DataProvider*)sourceOf(this);
    if (data->open(g_hMainWnd, rootOf(this)))
    {
        ContentProvider* failed = sourceOf(this);
        g_contentProvider = 0;
        if (failed)
            delete failed;
        sourceOf(this) = 0;
        return false;
    }
    TreeList* tree = treeOf(this);
    g_contentProvider = sourceOf(this);
    refresh(tree);
    return true;
}

void ClientNode::onInitItemMenu(HMENU hmenu, HWND tree, HTREEITEM item)
{
    LRESULT result = SendMessageW(tree, TVM_GETNEXTITEM, TVGN_CHILD, (LPARAM)item);
    if (result)
    {
        MENUITEMINFOW mii;
        memset(&mii.fMask, 0, 0x2C);
        mii.cbSize = sizeof(MENUITEMINFOW);
        mii.fMask = MIIM_STATE;
        mii.fState = 0;
        SetMenuItemInfoW(hmenu, 0xE102, FALSE, &mii);
        ContentProvider* source = sourceOf(this);
        BOOL enable = source && !source->isArchive();
        setMenuItemState(hmenu, 0xE105, enable);
        memset(&mii.fMask, 0, 0x2C);
        mii.cbSize = sizeof(MENUITEMINFOW);
        mii.fMask = MIIM_STATE;
        mii.fState = MFS_DISABLED;
        SetMenuItemInfoW(hmenu, 0xE106, FALSE, &mii);
    }
}
