#include "p05_common.h"
#include "ComboBox.h"

ComboBox::ComboBox()
    : Control32(), m_items(32, 32)
{
    m_dropLines = 3;
    m_flags = 0;
}

ComboBox::~ComboBox()
{
}
