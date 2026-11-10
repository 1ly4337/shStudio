#include "TransformWeaponModels.h"
#include "String.hpp"
#include "_imports_p07.h"

TransformWeaponModels::TransformWeaponModels()
    : ptrArray<TransformWeaponModel*>(32)
{
}

TransformWeaponModels::~TransformWeaponModels()
{
}

bool TransformWeaponModels::toText(String& out, DWORD fmt) const
{
    bool result = true;
    for (int i = 0; i < m_count; i++)
    {
        if (!result)
            break;
        result = m_items[i]->toText(out, fmt);
    }
    return result;
}
