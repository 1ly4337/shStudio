#include "GradeManager.h"
#include "GradeList.h"
#include "GradeSet.h"
#include "GradeSets.h"
#include "TreeListCtrl.h"
#include "Cell.h"
#include "Data_Items.h"
#include "Data_Item.h"
#include "iStream.h"
#include "ptrArray.h"
#include "_imports_p07.h"

static const CLIPFORMAT kItemRecordsFormat = 0x202;

GradeManager::GradeManager(GradeList* list, TreeListCtrl* ctrl)
    : DropTargetMgr(ctrl->m_hWnd)
{
    m_ctrl = ctrl;
    m_list = list;
    FORMATETC* format = (FORMATETC*)operator new(sizeof(FORMATETC));
    format->cfFormat = kItemRecordsFormat;
    format->ptd      = 0;
    format->dwAspect = DVASPECT_CONTENT;
    format->lindex   = -1;
    format->tymed    = TYMED_HGLOBAL;
    m_formats.add(format);
    m_hilite = 0;
}

GradeManager::~GradeManager()
{
}

void GradeManager::onDragLeave()
{
    HTREEITEM item = m_hilite;
    if (item)
    {
        GradeList* list = m_list;
        TVITEMW tvi;
        tvi.mask      = TVIF_STATE;
        tvi.hItem     = item;
        tvi.state     = 0;
        tvi.stateMask = TVIS_DROPHILITED;
        SendMessageW(list->m_tree.m_hWnd, TVM_SETITEMW, 0, (LPARAM)&tvi);
        RECT rc;
        InvalidateRect(list->m_tree.m_hWnd, list->m_tree.getItemRect(&rc, item), FALSE);
    }
}

bool GradeManager::onDragOver(DWORD keyState, POINT* clientPt, DWORD* effect)
{
    if (!DropTargetMgr::onDragOver(keyState, clientPt, effect))
        return false;

    TVHITTESTINFO hit;
    hit.pt.x  = clientPt->x;
    hit.pt.y  = clientPt->y;
    hit.flags = 0x46;
    hit.hItem = 0;
    HTREEITEM item = (HTREEITEM)SendMessageW(m_ctrl->m_hWnd, TVM_HITTEST, 0, (LPARAM)&hit);
    if (item != m_hilite)
    {
        if (m_hilite)
            m_list->setItemCut(m_hilite, 0);
        m_hilite = item;
    }
    if (m_list->gradeOf(m_hilite) < 0)
        return false;

    HTREEITEM hilite = m_hilite;
    GradeList* list = m_list;
    TVITEMW tvi;
    tvi.mask      = TVIF_STATE;
    tvi.hItem     = hilite;
    tvi.state     = TVIS_DROPHILITED;
    tvi.stateMask = TVIS_DROPHILITED;
    SendMessageW(list->m_tree.m_hWnd, TVM_SETITEMW, 0, (LPARAM)&tvi);
    RECT rc;
    InvalidateRect(list->m_tree.m_hWnd, list->m_tree.getItemRect(&rc, hilite), FALSE);
    *effect = DROPEFFECT_MOVE;
    return true;
}

bool GradeManager::onDrop(FORMATETC* format, STGMEDIUM* medium, DWORD* effect)
{
    int count = 0;
    if (format->cfFormat != kItemRecordsFormat || medium->tymed != TYMED_HGLOBAL)
        return true;
    HGLOBAL hMem = medium->hGlobal;
    if (!hMem)
        return true;
    short grade = m_list->gradeOf(m_hilite);
    if (grade < 0)
        return true;

    iStream in(false, 1200);
    SIZE_T size = GlobalSize(hMem);
    void* data = GlobalLock(hMem);
    if (data)
    {
        in.assign(data, (int)size);
        GlobalUnlock(hMem);
    }
    try
    {
        count = (int)in.readInt();
        for (int i = 0; i < count; ++i)
        {
            Data::Item record(i, 0x600);
            record.read(in, 'clip');
            Data::Item* item = m_list->m_data->sub_42FC00(FIELD(BYTE, &record, 0x54) ,
                                                          FIELD(BYTE, &record, 0x55) );
            if (item)
            {
                GradeSet* old = m_list->m_sets.find(FIELD(short, item, 0xA4) );
                if (old)
                {
                    old->detachItem(item);
                    if (item->m_data)
                        SendMessageW(m_list->m_tree.m_hWnd, TVM_DELETEITEM, 0, (LPARAM)item->m_data);
                }
                FIELD(short, item, 0xA4) = grade;
                GradeSet* set = m_list->m_sets.find(grade);
                if (!set)
                {
                    set = new GradeSet((unsigned short)grade);
                    m_list->m_sets.add(set);
                }
                set->add(item);
                HTREEITEM parent = set->m_hItem;
                GradeList* list = m_list;
                list->insertCell(new Cell(item), parent, -1, 0, TVI_LAST);
            }
        }
    }
    catch (...)
    {
    }
    SendMessageW(m_ctrl->m_hWnd, TVM_SELECTITEM, TVGN_DROPHILITE, 0);
    return true;
}
