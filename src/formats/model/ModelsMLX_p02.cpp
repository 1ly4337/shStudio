#include "p02_common.h"
#include "ModelsMLX.h"
#include "ModelMLX.h"

bool ModelsMLX::toText(String& out, DWORD fmt) const
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

void ModelsMLX::read(iStream& in, DWORD )
{
    deleteAll();
    try
    {
        DWORD format = '1XLM';
        int n = in.readInt();
        if ((n & 0xFFFFFF) == 0x584C4D)
        {
            format = (DWORD)n;
            n = in.readInt();
        }
        for (int i = 0; i < n; ++i)
        {
            ModelMLX* model = new ModelMLX(0);
            model->read(in, format);
            add(model);
        }
    }
    catch (...)
    {
        deleteAll();
    }
}

void ModelsMLX::write(oStream& out, DWORD ) const
{
    out << (unsigned long)'2XLM';
    out << (unsigned long)m_count;
    for (int i = 0; i < m_count; ++i)
    {
        m_items[i]->m_id = i;
        m_items[i]->write(out, '2XLM');
    }
}
