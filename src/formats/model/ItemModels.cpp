#include "ItemModels.h"
#include "String.hpp"
#include "_imports_p06.h"

ItemModels::ItemModels()
    : ptrArray<ItemModel*>()
{
}

ItemModels::~ItemModels()
{
}

bool ItemModels::toText(String& out, DWORD fmt) const
{
    bool ok = true;
    for (int i = 0; i < m_count && ok; ++i)
        ok = m_items[i]->toText(out, fmt);
    return ok;
}
