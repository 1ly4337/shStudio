#include "p05_common.h"
#include "HWndVec.h"

HWndVec::~HWndVec()
{
    for (int i = 0; i < m_capacity; ++i)
        resetItem(&m_items[i]);
    m_count = 0;
}
