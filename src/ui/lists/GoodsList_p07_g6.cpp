#include "GoodsList.h"
#include "GoodsEditor.h"
#include "ComboBox.h"
#include "_imports_p07.h"

void GoodsList::fillTypeCombo(ComboBox* combo)
{
    int count = m_types2.m_count;
    for (int i = 0; i < count; ++i)
    {
        unsigned char type = (i < 0 || i >= m_types2.m_capacity) ? 0 : m_types2.m_items[i];
        const wchar_t* name = goodsTypeName(type);
        if (name)
            addTypeItem(combo, type, name);
    }
    count = m_types.m_count;
    for (int i = 0; i < count; ++i)
    {
        unsigned char type = (i < 0 || i >= m_types.m_capacity) ? 0 : m_types.m_items[i];
        const wchar_t* name = goodsTypeName(type);
        if (name)
            addTypeItem(combo, type, name);
    }
}
