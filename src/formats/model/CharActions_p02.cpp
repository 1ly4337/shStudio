#include "p02_common.h"
#include "CharActions.h"
#include "CharAction.h"

void CharActions::read(iStream& in, DWORD fmt)
{
    deleteAll();
    if (in.m_size - in.m_pos < 7)
        throw 0;
    if (in.m_size - in.m_pos < 3)
        throw ArrayIndexOutOfBoundsException(in.m_size - in.m_pos, 3);
    BYTE* p = in.m_data + in.m_pos;
    WORD signature = *(WORD*)p;
    BYTE version = p[2];
    in.m_pos += 3;
    if (signature != 0x4C41 || version != '3')
        throw 1;
    int n = in.readInt();
    if (!n)
        throw 2;
    CharAction* current = 0;
    try
    {
        for (int i = 0; i < n; ++i)
        {
            current = 0;
            CharAction* action = new CharAction(i, 0);
            current = action;
            action->read(in, fmt);
            add(action);
        }
    }
    catch (...)
    {
        delete current;
        throw 3;
    }
}

void CharActions::write(oStream& out, DWORD fmt) const
{
    out.put("AL3", 3);
    out << (unsigned long)m_count;
    for (int i = 0; i < m_count; ++i)
        m_items[i]->write(out, fmt);
}

bool CharActions::toText(String& out, DWORD fmt) const
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
