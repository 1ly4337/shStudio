#include "p05_common.h"
#include "CtrlsPanel.h"

CtrlsPanel::CtrlsPanel(Popup* doc, glViewer* viewer)
    : Container(), glControls(), m_doc(doc), m_viewer(viewer)
{
    m_bkColor = panelBackgroundColor();
    m_updating = false;
    m_bottom = 0;
}

CtrlsPanel::~CtrlsPanel()
{
}
