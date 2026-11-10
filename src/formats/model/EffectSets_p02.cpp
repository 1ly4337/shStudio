#include "p02_common.h"
#include "EffectSets.h"
#include "EffectSet.h"

void EffectSets::read(iStream& in, DWORD fmt)
{
    in.read(&m_field2C, 16);
    int n = (int)in.readInt();
    for (int i = 0; i < n; ++i)
    {
        EffectSet* set = new EffectSet(0);
        set->read(in, fmt);
        add(set);
    }
}

void EffectSets::write(oStream& out, DWORD fmt) const
{
    out.put(&m_field2C, 16);
    out << (unsigned long)m_count;
    for (int i = 0; i < m_count; ++i)
        m_items[i]->write(out, fmt);
}

int EffectSets::vslot13(int arg)
{
    int result = 0;
    for (int i = 0; i < m_count; ++i)
        m_items[i]->dump(*(String*)arg);
    return result;
}
