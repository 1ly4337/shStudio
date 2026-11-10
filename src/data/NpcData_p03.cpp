#include "NpcData.h"
#include "QuestsDetails.h"
#include "QuestDetails.h"
#include "String.hpp"
#include "iStream.hpp"
#include "oStream.hpp"
#include "_imports_w0.h"
#include "p03_free.h"

void NpcData::clear()
{
    m_format = 0;
    NpcPerso::clear();
    if (m_bytes.m_data && m_bytes.m_capacity)
        memset(m_bytes.m_data, 0, m_bytes.m_capacity);
    m_bytes.m_size = 0;
    m_bytes.m_pos = 0;
    m_quests.deleteAll();
    m_isNew = true;
}

void NpcData::read(iStream& s, DWORD tag)
{
    if (tag != 0x6C747874 )
        clear();
    NpcPerso::read(s, tag);
    m_isNew = false;
    if (tag != 0x6C747874 )
    {
        int start = s.m_pos;
        short a, b;
        do
        {
            while ((unsigned short)s.readShort() <= 0x3E8u)
                ;
            a = s.readShort();
            b = s.readShort();
        } while (a || b != 1);
        if (s.m_pos >= 6)
            s.m_pos -= 6;
        if (s.m_size - s.m_pos < 4)
            throw;
        int size = s.m_pos - start;
        if (start >= 0 && start <= s.m_size)
            s.m_pos = start;
        s.read(m_bytes, size);
    }
    m_quests.read(s, tag);
    if (tag != 0x6C747874 )
        m_format = m_quests.m_version;
}

void NpcData::write(oStream& s, DWORD tag) const
{
    if (s.m_data && s.m_capacity)
        memset(s.m_data, 0, s.m_capacity);
    s.m_size = 0;
    s.m_pos = 0;
    s.m_writeNul = 0;
    if (tag != 0x6C747874 )
    {
        sortList(const_cast<StandardNPCs&>(m_npcs[1]), 0x75696420 , 0, true);
        sortList(const_cast<StandardNPCs&>(m_npcs[2]), 0x75696420 , 0, true);
        sortList(const_cast<QuestsDetails&>(m_quests), 0x71756964 , 0, true);
    }
    for (int k = 0; k < 14; ++k)
        const_cast<StandardNPCs&>(m_npcs[k]).writeList(s, tag);
    if (tag != 0x6C747874 )
        s.put(m_bytes.m_data + m_bytes.m_pos, m_bytes.m_size - m_bytes.m_pos);
    const_cast<QuestsDetails&>(m_quests).writeList(s, tag);
}

int NpcData::getCodePage() const
{
    if (!m_fileName)
        return -1;
    String path;
    path.append(m_fileName, -1);
    path.removeExtension();
    int codePage;
    if (path.endsWithNoCase(L"FRANCE")
        || path.endsWithNoCase(L"GERMANY")
        || path.endsWithNoCase(L"ITALY")
        || path.endsWithNoCase(L"USA"))
        codePage = 1252;
    else if (path.endsWithNoCase(L"POLAND") || path.endsWithNoCase(L"TURKEY"))
        codePage = 1250;
    else if (path.endsWithNoCase(L"CHINA"))
        codePage = 936;
    else if (path.endsWithNoCase(L"KOREA"))
        codePage = 949;
    else
        codePage = -1;
    return codePage;
}

HRESULT NpcData::setCodePage(int version)
{
    m_format = version;
    m_quests.m_version = version;
    for (int i = 0; i < m_quests.m_count; ++i)
        m_quests.m_items[i]->m_version = version;
    return 0;
}
