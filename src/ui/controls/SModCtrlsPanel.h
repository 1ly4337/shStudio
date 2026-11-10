#pragma once
#include "CtrlsPanel.h"
#include "ComboBox.h"
#include "Button.h"

class SModCtrlsPanel : public CtrlsPanel {
public:
    SModCtrlsPanel(Popup* doc, glViewer* viewer);
    ~SModCtrlsPanel() override;
    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;

    ComboBox m_combo1;
    ComboBox m_combo2;
    Button   m_btn1;
    Button   m_btn2;
};
static_assert(sizeof(SModCtrlsPanel) == 0x24C, "SModCtrlsPanel size");
