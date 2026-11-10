#include "EffectSet.h"

EffectSet::Effect::Effect()
{
    m_id = 0;
    m_value04 = 0;
    m_value08 = 0;
    m_value0C = 0;
    m_value10 = 0;
    m_value14 = 0;
    for (int i = 0; i < 6; ++i)
        m_values[i] = 0;
    m_value60 = 0;
}

void EffectSet::Effect::read(iStream& in, DWORD )
{
    m_name.clear();
    if (in.m_size - in.m_pos < 68)
        throw 0;
    m_id = in.readInt();
    m_value04 = in.readFloat();
    m_value08 = in.readInt();
    m_value0C = in.readInt();
    m_value10 = in.readInt();
    m_value14 = in.readFloat();
    m_name.read(in, 4, 1200);
    for (int i = 0; i < 6; ++i)
        m_values[i] = in.readInt();
    m_pos.read(in, 'file');
    m_value60 = in.readInt();
}

void EffectSet::Effect::write(oStream& out, DWORD ) const
{
    out << (unsigned long)m_id << m_value04 << (unsigned long)m_value08 << (unsigned long)m_value0C
        << (unsigned long)m_value10 << m_value14;
    m_name.write(out, 4, 1200);
    for (int i = 0; i < 6; ++i)
        out << (unsigned long)m_values[i];
    m_pos.write(out, 'file');
    out << (unsigned long)m_value60;
}

void EffectSet::Effects::clear()
{
    deleteAll();
}

void EffectSet::Effects::read(iStream& in, DWORD fmt)
{
    clear();
    try
    {
        int n = (int)in.readInt();
        if (in.m_size - in.m_pos < 68 * n)
            throw 0;
        for (int i = 0; i < n; ++i)
        {
            Effect* effect = new Effect();
            try
            {
                effect->read(in, fmt);
            }
            catch (...)
            {
                if (effect)
                    delete effect;
                throw;
            }
            add(effect);
        }
    }
    catch (...)
    {
        in.m_pos = 0;
        throw;
    }
}

void EffectSet::Effects::write(oStream& out, DWORD fmt) const
{
    out << (unsigned long)m_count;
    for (int i = 0; i < m_count; ++i)
        m_items[i]->write(out, fmt);
}

void EffectSet::Effects::toText(String& out) const
{
    for (int i = 0; i < m_count; ++i)
    {
        const Effect* e = m_items[i];
        out << (long)e->m_id << L'\t' << e->m_value04 << L'\t' << (long)e->m_value08 << L'\t'
            << (long)e->m_value0C << L'\t' << (long)e->m_value10 << L'\t' << e->m_value14 << L'\t';
        out << L"{ ";
        for (int k = 0; k < 5; ++k)
            out << (long)e->m_values[k] << L", ";
        out << (long)e->m_values[5] << L" }";
        out.m_radix = 10;
        out << L'\t';
        String& s = appendFPoint(out, &e->m_pos) << L'\t' << (long)e->m_value60 << L'\t';
        s.append(e->m_name.m_buffer, e->m_name.m_length);
        s.newline();
    }
}

EffectSet::EffectSet(int )
    : m_id(0)
{
}

EffectSet::~EffectSet()
{
}

void EffectSet::read(iStream& in, DWORD fmt)
{
    m_id = in.readInt();
    m_effects.read(in, fmt);
}

void EffectSet::write(oStream& out, DWORD fmt) const
{
    out << (unsigned long)m_id;
    m_effects.write(out, fmt);
}

int EffectSet::getInt(DWORD tag) const
{
    if (tag == 'id  ')
        return m_id;
    return -1;
}

void EffectSet::reset()
{
    m_effects.clear();
}

int EffectSet::compare(const Node* other, DWORD tag) const
{
    const EffectSet* p = dynamic_cast<const EffectSet*>(other);
    if (!p)
        return 1;
    if (tag != 'id  ')
        return 0;
    if (m_id != p->m_id)
        return 2 * (m_id >= p->m_id) - 1;
    return 0;
}

void EffectSet::dump(String& out)
{
    out << (long)m_id;
    ++out.m_indent;
    out << L'\t';
    out << L"\r\n";
    m_effects.toText(out);
    p02_unindent(out);
}
