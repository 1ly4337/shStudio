#pragma once
#include <windows.h>
#include "DocView.h"
#include "TacticsList.h"

class TacticsZones;

class TacticsView : public DocView
{
public:
    TacticsView(FileTree* data, TacticsZones* zones);
    virtual ~TacticsView();

    BOOL invalidate(bool erase) override
    {
        return InvalidateRect(m_list.m_hWnd, 0, erase);
    }
    LPCWSTR getClassName() override { return L"TacticsView"; }
    bool onCreate() override;
    void onActivate(bool active) override { m_list.onActivate(active); }
    int getMenuId() override { return 177; }
    virtual int vslot100() { return m_list.setItemCount(0); }
    virtual void editZone(int index);

public:
    TacticsZones* m_zones;
    TacticsList   m_list;
};
static_assert(sizeof(TacticsView) == 0x1A8, "TacticsView size");
