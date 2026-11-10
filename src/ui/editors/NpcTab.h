#pragma once
#include <windows.h>
#include "TabCtrl.h"
#include "NpcList.h"

class NpcView;
class NpcData;

class NpcTab : public TabCtrl<NpcList*>
{
public:
    explicit NpcTab(NpcView* view) : m_view(view), m_data(0) {}
    virtual ~NpcTab() {}

    void onActivate(bool active) override;
    DWORD getTabStyle() override { return 1024; }
    bool createPages(RECT* rc) override;

    void fillLists();

public:
    NpcView* m_view;
    NpcData* m_data;
};
