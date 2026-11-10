#include <windows.h>
#include "MonstersSet.h"
#include "Monster.h"

MonstersSet::~MonstersSet()
{
    for (int i = 0; i < m_count; ++i)
        m_items[i] = 0;
    m_count = 0;
}
