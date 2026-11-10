#include "p05_common.h"
#include "DocView.h"

DocView::DocView(FileTree* data, Serializable* content)
    : Document(data), StreamCallback(), m_search()
{
    m_content = content;
    m_searchEnabled = true;
    m_margin = 0;
}

DocView::~DocView()
{
}

LRESULT DocView::onStreamProgress(WPARAM pos, LPARAM range)
{
    return setProgress(pos, range);
}
