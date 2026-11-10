#include "p05_common.h"
#include "MonsterModels.h"

MonsterModels::MonsterModels()
    : Node(), ptrArray<MonsterModel*>(256), Serializable()
{
    m_signature = 'MO2 ';
}

MonsterModels::~MonsterModels()
{
}

const wchar_t* MonsterModels::getString(DWORD tag) const
{
    const wchar_t* result = L"NPC Models";
    if (tag != 'name')
        return 0;
    return result;
}

void MonsterModels::reset()
{
    static_cast<Serializable*>(this)->clear();
}

int MonsterModels::compare(const Node* , DWORD ) const
{
    return 1;
}
