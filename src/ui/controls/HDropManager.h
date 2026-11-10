#pragma once
#include <windows.h>
#include <ole2.h>
#include "DropTargetMgr.h"

class WorkspaceList;

class HDropManager : public DropTargetMgr
{
public:
    HDropManager(WorkspaceList* owner, void* tree);

    virtual ~HDropManager();
    virtual bool onDragOver(DWORD keyState, POINT* pt, DWORD* effect) override;
    virtual bool onDrop(FORMATETC* format, STGMEDIUM* medium, DWORD* effect) override;
    virtual void onDragLeave() override;

public:
    WorkspaceList* m_owner;
    void*          m_tree;
};
static_assert(sizeof(HDropManager) == 0x4C, "HDropManager size");
