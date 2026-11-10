#include "p05_common.h"
#include "SearchPanel.h"

SearchPanel::SearchPanel()
    : Window()
{
    m_bkColor = panelBackgroundColor();
    m_status = 0;
    m_edit = 0;
    m_button = 0;
}

void SearchPanel::onMove(int , int )
{
    invalidate(true);
}
