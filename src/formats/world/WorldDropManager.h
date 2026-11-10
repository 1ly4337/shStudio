#pragma once
#include <windows.h>
#include <ole2.h>
#include "DropTargetMgr.h"
#include "kWorld_Items.h"

class String;
class WorldList;
class World;

class WorldDropManager : public DropTargetMgr
{
public:
    WorldDropManager(WorldList* owner, World* world);

    virtual ~WorldDropManager();
    virtual bool onDragOver(DWORD keyState, POINT* clientPt, DWORD* effect) override;
    virtual bool onDrop(FORMATETC* format, STGMEDIUM* medium, DWORD* effect) override;
    virtual void onDragLeave() override;

    static bool splitPath(String& path, String& head);
    static kWorld::Section* findSection(String& path, kWorld::Items* world, int* kind);

public:
    World*     m_world;
    int        m_kind;
    WorldList* m_owner;
    HWND       m_tree;
    LPARAM     m_hilite;
};
static_assert(sizeof(WorldDropManager) == 0x58, "WorldDropManager size");
