#include <windows.h>
#include <ole2.h>
#include <shellapi.h>
#include <commctrl.h>
#include "HDropManager.h"
#include "WorkspaceList.h"
#include "Node.h"
#include "FileTree.h"
#include "FolderTree.h"
#include "DataProvider.h"
#include "ArchiveProvider.h"
#include "ida_compat.h"
#include "_imports_p09.h"
#include "_imports_g5.h"

HDropManager::HDropManager(WorkspaceList* owner, void* tree)
    : DropTargetMgr(FIELD(HWND, tree, 0x04))
{
    m_owner = owner;
    m_tree = tree;
    FORMATETC* format = (FORMATETC*)operator new(sizeof(FORMATETC));
    format->cfFormat = CF_HDROP;
    format->ptd = 0;
    format->dwAspect = DVASPECT_CONTENT;
    format->lindex = -1;
    format->tymed = TYMED_HGLOBAL;
    m_formats.add(format);
}

HDropManager::~HDropManager()
{
}

bool HDropManager::onDragOver(DWORD keyState, POINT* pt, DWORD* effect)
{
    bool result = DropTargetMgr::onDragOver(keyState, pt, effect);
    if (!result)
        return result;
    TVHITTESTINFO hit;
    hit.pt = *pt;
    hit.flags = 70;
    hit.hItem = 0;
    LRESULT item = SendMessageW(FIELD(HWND, m_tree, 0x04), TVM_HITTEST, 0, (LPARAM)&hit);
    SendMessageW(FIELD(HWND, m_tree, 0x04), TVM_SELECTITEM, TVGN_DROPHILITE, item);
    return true;
}

void HDropManager::onDragLeave()
{
    SendMessageW(FIELD(HWND, m_tree, 0x04), TVM_SELECTITEM, TVGN_DROPHILITE, 0);
}

bool HDropManager::onDrop(FORMATETC* format, STGMEDIUM* medium, DWORD* effect)
{
    FolderTree* folder = 0;
    ArchiveProvider* archive = 0;
    if (*effect == DROPEFFECT_LINK && ((void*)g_studioFrame))
    {
        if ((*(bool(__thiscall**)(void*))(*(int*)((void*)g_studioFrame) + 0x170))(((void*)g_studioFrame)))
        {
            TVHITTESTINFO hit;
            hit.pt = m_point;
            hit.flags = 70;
            hit.hItem = 0;
            LRESULT item = SendMessageW(FIELD(HWND, m_tree, 0x04), TVM_HITTEST, 0, (LPARAM)&hit);
            Node* node = (*(Node*(__thiscall**)(void*, LRESULT))(*(int*)m_owner + 0x10C))(m_owner, item);
            FileTree* file = dynamic_cast<FileTree*>(node);
            if (file)
                folder = FIELD(FolderTree*, file, 0x0C);
            else
                folder = dynamic_cast<FolderTree*>(node);
            archive = dynamic_cast<ArchiveProvider*>(FIELD(DataProvider*, ((void*)g_studioFrame), 0x170));
        }
    }
    if (format->cfFormat == CF_HDROP && medium->tymed == TYMED_HGLOBAL)
    {
        HDROP hDrop = (HDROP)GlobalLock(medium->hGlobal);
        if (hDrop)
        {
            UINT index = 0;
            UINT count = DragQueryFileW(hDrop, 0xFFFFFFFF, 0, 0);
            if (count)
            {
                do
                {
                    WCHAR fileName[MAX_PATH];
                    DragQueryFileW(hDrop, index, fileName, 0x208);
                    if (archive && folder)
                    {
                        int error = archive->addFile((Window*)m_owner, folder, fileName);
                        if (error)
                        {
                            m_owner->reportDropError(error);
                            break;
                        }
                    }
                    else if (!(*(bool(__thiscall**)(void*, UINT, POINT*, WCHAR*))(*(int*)m_tree + 0x50))(
                                 m_tree, index, &m_point, fileName))
                    {
                        break;
                    }
                    ++index;
                } while (index < count);
            }
        }
        GlobalUnlock(medium->hGlobal);
    }
    return SendMessageW(FIELD(HWND, m_tree, 0x04), TVM_SELECTITEM, TVGN_DROPHILITE, 0) != 0;
}
