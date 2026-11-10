#include "MobsSkills.h"
#include "iStream.h"

MobsSkills::MobsSkills()
    : ptrArray<MobsSkill*>(64)
{
    m_format = 0x500;
}

MobsSkills::~MobsSkills()
{
}

bool MobsSkills::isEmpty() const
{
    return size() == 0;
}

void MobsSkills::clear()
{
    deleteAll();
}

const wchar_t* MobsSkills::getFilePrefix() const
{
    return L"BinarySData\\DBNpcSkillText_";
}

void MobsSkills::load(iStream& in, int arg2, int arg3)
{
    read(in, 'ltxt');
}

int MobsSkills::serialV5() const
{
    return m_format;
}
