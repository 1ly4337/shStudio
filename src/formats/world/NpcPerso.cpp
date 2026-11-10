#include "Preferences.h"
#include "NpcPerso.h"
#include "StandardNPC.h"
#include "iStream.hpp"
#include "oStream.hpp"
#include "_imports_w0.h"

NpcPerso::NpcPerso()
{
    m_isNew = true;
    for (int type = 1; type < 14; ++type)
        m_npcs[type].m_type = type;
}

NpcPerso::~NpcPerso()
{
}

bool NpcPerso::isEmpty() const
{
    return m_isNew;
}

void NpcPerso::clear()
{
    m_isNew = true;
    for (int k = 0; k < 14; ++k)
        m_npcs[k].deleteAll();
}

StandardNPC* NpcPerso::find(unsigned char type, short id)
{
    if ((unsigned char)(type - 1) > 12)
        return 0;
    StandardNPCs* list = &m_npcs[type];
    if (!list)
        return 0;
    int count = list->m_count;
    int i = 0;
    if (count <= 0)
        return 0;
    for (StandardNPC** p = list->m_items; (*p)->m_id != id; ++p)
    {
        if (++i >= count)
            return 0;
    }
    return list->m_items[i];
}

void NpcPerso::read(iStream& s, DWORD tag)
{
    bool cleanCR = g_preferences.getInt(L"NPCQuest", L"clean CR", 1) == 1;
    switch (tag)
    {
    case 0x66696C65 :
        for (int i = 1; i < 14; ++i)
        {
            m_npcs[i].readList(s, 0x66696C65 );
            if (cleanCR)
                m_npcs[i].cleanCR();
        }
        break;
    case 0x666C6E74 :
        for (int j = 1; j < 14; ++j)
            m_npcs[j].readList(s, 0x666C6E74 );
        break;
    case 0x6C747874 :
        for (int k = 1; k < 14; ++k)
        {
            m_npcs[k].readList(s, 0x6C747874 );
            if (cleanCR)
                m_npcs[k].cleanCR();
        }
        break;
    }
}

void NpcPerso::write(oStream& s, DWORD tag) const
{
    for (int k = 0; k < 14; ++k)
        const_cast<StandardNPCs&>(m_npcs[k]).writeList(s, tag);
}
