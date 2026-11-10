#pragma once
#include <windows.h>
#include <ole2.h>
#include <commctrl.h>
#include "DropTargetMgr.h"

class GradeList;
class TreeListCtrl;

class GradeManager : public DropTargetMgr
{
public:
    GradeManager(GradeList* list, TreeListCtrl* ctrl);
    virtual ~GradeManager();

    virtual bool onDragOver(DWORD keyState, POINT* clientPt, DWORD* effect);
    virtual bool onDrop(FORMATETC* format, STGMEDIUM* medium, DWORD* effect);
    virtual void onDragLeave();

public:
    TreeListCtrl* m_ctrl;
    GradeList*    m_list;
    HTREEITEM     m_hilite;
};
static_assert(sizeof(GradeManager) == 0x50, "GradeManager size");
