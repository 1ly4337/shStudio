#include "Names.h"
#include "Name.h"
#include "iStream.hpp"
#include "oStream.hpp"
#include "String.hpp"
#include "_imports_w0.h"

void Names::read(iStream& s, DWORD tag)
{
    deleteAll();
    if (s.m_size - s.m_pos < 4)
        throw 1;
    int count = s.readInt();
    if (count < 0 || count > (s.m_size - s.m_pos) / 256)
        throw 2;
    for (int i = 0; i < count; ++i)
    {
        Name* name = new Name(i, m_owner);
        name->read(s, tag);
        add(name);
    }
}

void Names::write(oStream& s, DWORD tag) const
{
    (s << (unsigned long)(m_count));
    for (int i = 0; i < m_count; ++i)
        m_items[i]->write(s, tag);
}

bool Names::toText(String& out, DWORD tag) const
{
    bool ok = true;
    for (int i = 0; i < m_count; ++i)
    {
        if (!ok)
            break;
        ok = m_items[i]->toText(out, tag);
        out.newline();
    }
    return ok;
}

const wchar_t* Names::nameOf(int id)
{
    int count = m_count;
    int i = 0;
    if (count <= 0)
        return 0;
    for (Name** p = m_items; (*p)->m_id != id; ++p)
    {
        if (++i >= count)
            return 0;
    }
    return m_items[i]->m_file.m_buffer;
}
