#include "p05_common.h"
#include "StdTextView.h"

StdTextView::StdTextView()
    : TextView(&m_file, true),
      m_file(0, 0, 0, 0, 0, 0),
      m_content()
{
}

StdTextView::~StdTextView()
{
}

bool StdTextView::loadText()
{
    setContent(m_content);
    return true;
}
