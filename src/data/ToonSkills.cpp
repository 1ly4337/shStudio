#include "ToonSkills.h"
#include "iStream.hpp"
#include "_imports_p09.h"
#include "_imports_g1b.h"

ToonSkills::ToonSkills()
    : ptrArray<ToonSkill*>(128)
{
    m_format = 0x500;
}

ToonSkills::~ToonSkills()
{
}

bool ToonSkills::isEmpty() const
{
    return size() == 0;
}

void ToonSkills::clear()
{
    deleteAll();
}

const wchar_t* ToonSkills::getFilePrefix() const
{
    return L"BinarySData\\DBSkillText_";
}

void ToonSkills::load(iStream& in, int, int)
{
    read(in, 'ltxt');
}

ToonSkill* ToonSkills::findById(int skillId) const
{
    int count = m_count;
    if (count <= 0)
        return 0;
    int i = 0;
    while (m_items[i]->m_skillId != skillId)
    {
        if (++i >= count)
            return 0;
    }
    return m_items[i];
}
