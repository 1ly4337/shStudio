#include "Preferences.h"
#include "StandardNPCs.h"
#include "Merchant.h"
#include "GateKeeper.h"
#include "_w2_inl.h"

StandardNPC* StandardNPCs::create(int type)
{
    if (type == 1)
        return new Merchant();
    if (type == 2)
        return new GateKeeper();
    return new StandardNPC(type);
}

void StandardNPCs::readList(iStream& s, int tag)
{
    if (tag == 0x6C747874 ) {
        int n = s.readInt();
        for (int i = 0; i < n; ++i) {
            if (i < m_count) {
                StandardNPC* npc = m_items[i];
                static_cast<Streamable*>(npc)->read(s, 0x6C747874 );
            } else if (m_type == 1) {
                Merchant tmp;
                tmp.Merchant::read(s, 0x6C747874 );
            } else if (m_type == 2) {
                GateKeeper tmp;
                tmp.GateKeeper::read(s, 0x6C747874 );
            } else {
                StandardNPC tmp(m_type);
                tmp.readMain(s, 0x6C747874 );
                tmp.readLists(s, 0x6C747874 );
            }
        }
        return;
    }

    this->deleteAll();
    m_18 = 0x500;
    if (tag == 0x666C6E74 )
        m_18 = 0x2600;
    try {
        int n = s.readInt();
        for (int i = 0; i < n; ++i) {
            StandardNPC* npc = create(m_type);
            if (!npc)
                throw 0;
            try {
                static_cast<Streamable*>(npc)->read(s, tag);
                this->add(npc);
            } catch (...) {
                delete npc;
                throw;
            }
        }
    } catch (...) {
        this->deleteAll();
        throw;
    }
}

void StandardNPCs::writeList(oStream& s, int tag)
{
    s << (unsigned long)m_count;
    for (int i = 0; i < m_count; ++i) {
        StandardNPC* npc = m_items[i];
        static_cast<Streamable*>(npc)->write(s, tag);
    }
}

void StandardNPCs::cleanCR()
{
    if (g_preferences.getInt(L"NPCQuest", L"clean CR", 1) == 1) {
        for (int i = 0; i < m_count; ++i)
            m_items[i]->trimStrings();
    }
}

void StandardNPCs::clearAllLists()
{
    for (int i = 0; i < m_count; ++i)
        m_items[i]->clearLists();
}

void StandardNPCs::buildAllListStrings()
{
    for (int i = 0; i < m_count; ++i)
        m_items[i]->buildListStrings();
}
