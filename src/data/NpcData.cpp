#include "NpcData.h"
#include "QuestDetails.h"

NpcData::NpcData()
{
}

NpcData::~NpcData()
{
}

void NpcData::load(iStream& in, int arg2, int arg3)
{
    read(in, 'ltxt');
}

const wchar_t* NpcData::getFilePrefix() const
{
    return L"Npc\\NpcQuestTrans_";
}
