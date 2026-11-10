#include "p05_common.h"
#include "SModCtrlsPanel.h"

SModCtrlsPanel::SModCtrlsPanel(Popup* doc, glViewer* viewer)
    : CtrlsPanel(doc, viewer),
      m_combo1(), m_combo2(), m_btn1(), m_btn2()
{
}

SModCtrlsPanel::~SModCtrlsPanel()
{
}
