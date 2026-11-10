#pragma once
#include <windows.h>
#include "CtrlsPanel.h"
#include "EditText.h"

class Vani;

class VaniCtrlsPanel : public CtrlsPanel
{
public:
    VaniCtrlsPanel(Popup* doc, glViewer* viewer) : CtrlsPanel(doc, viewer) {}
    virtual ~VaniCtrlsPanel() {}

    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;

    void setVani(Vani* vani);

public:
    EditText m_subItems;
    EditText m_steps;
};
static_assert(sizeof(VaniCtrlsPanel) == 0x1FC, "VaniCtrlsPanel size");
