#include "p02_common.h"
#include "ItemsSet.h"
#include "Data_Item.h"

namespace Data {

ItemsSet::~ItemsSet()
{
    for (int i = 0; i < m_count; ++i)
        m_items[i] = 0;
    m_count = 0;
}

void ItemsSet::reset()
{
    for (int i = 0; i < m_count; ++i)
        m_items[i] = 0;
    m_count = 0;
}

int ItemsSet::compare(const Node* other, DWORD tag) const
{
    const ItemsSet* p = dynamic_cast<const ItemsSet*>(other);
    if (!p)
        return 1;
    if (tag != 'cnt ')
        return 0;
    if (m_count != p->m_count)
        return 2 * (m_count >= p->m_count) - 1;
    return 0;
}

}
