#include "p04_common.h"
#include "TacticsZones.h"
#include "TacticsZone.h"
#include "iStream.h"
#include "oStream.h"
#include "String.hpp"

void TacticsZones::read(iStream& in, DWORD fmt)
{
    m_version = (int)in.readInt();
    Collection<TacticsZone>::read(in, fmt);
}

void TacticsZones::write(oStream& out, DWORD fmt) const
{
    out.m_writeNul = false;
    out << (unsigned long)m_version;
    Collection<TacticsZone>::write(out, fmt);
}

bool TacticsZones::toText(String& out, DWORD fmt) const
{
    if (m_count <= 0)
        return true;
    wchar_t sep = p04_separator(fmt);
    int i = 0;
    do
    {
        out << L'#';
        int next = i + 1;
        out << (long)next << sep;
        m_items[i]->toText(out, fmt);
        out.newline();
        i = next;
    } while (i < m_count);
    return true;
}
