#include "ModelsMLT.h"
#include "String.hpp"
#include "_imports_p06.h"

ModelsMLT::ModelsMLT()
    : ptrArray<ModelMLT*>(32)
{
}

ModelsMLT::~ModelsMLT()
{
}

bool ModelsMLT::toText(String& out, DWORD fmt) const
{
    bool result = true;
    for (int i = 0; i < m_count; ++i)
    {
        if (!result)
            break;
        result = m_items[i]->toText(out, fmt);
    }
    return result;
}
