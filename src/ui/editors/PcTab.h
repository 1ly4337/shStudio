#pragma once
#include "TabControl.h"
#include "StringList.h"
#include "PcList.h"

class PcView;
class CapeModels;

class PcTab : public TabControl
{
public:
    PcTab(PcView* view);
    virtual ~PcTab();

    HWND getPage(int index) override;
    DWORD getTabStyle() override;
    bool createPages(RECT* rc) override;

public:
    PcView*     m_view;
    CapeModels* m_models;
    PcList      m_modelList;
    StringList  m_capeList;
    StringList  m_shoulderList;
};
static_assert(sizeof(PcTab) == 0x320, "PcTab size");
