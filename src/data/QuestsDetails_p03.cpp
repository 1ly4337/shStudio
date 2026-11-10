#include "Preferences.h"
#include "QuestsDetails.h"
#include "QuestDetails.h"
#include "_w2_inl.h"

void QuestsDetails::read(iStream& s, int tag)
{
    int savedPos = static_cast<aStream&>(s).m_pos;
    int versions[3] = { 0x500, 0x600, 0 };
    g_questCleanCR = g_preferences.getInt(L"NPCQuest", L"clean CR", 1) == 1;
    int idx = 0;
    for (;;) {
        int v = versions[idx];
        m_version = v;
        if (!v)
            throw 0;
        if (v == 0x500 && tag != 0x66696C65 ) {
            ++idx;
            m_version = 0x600;
        }
        if (tag == 0x6C747874 ) {
            int n = w2_readIntInline(s);
            for (int k = 0; k < n; ++k) {
                if (k < m_count) {
                    QuestDetails* q = m_items[k];
                    static_cast<Streamable*>(q)->read(s, 0x6C747874 );
                }
            }
            ++idx;
            continue;
        }
        if (m_items) {
            for (int k = 0; k < m_count; ++k) {
                QuestDetails* q = m_items[k];
                if (q) {
                    this->destroyItem(q);
                    m_items[k] = 0;
                }
            }
            m_count = 0;
        }
        aStream& a = static_cast<aStream&>(s);
        if (savedPos >= 0 && savedPos <= a.m_size)
            a.m_pos = savedPos;
        QuestDetails* obj = 0;
        if (tag == 0x666C6E74 )
            m_version |= 0x2000;
        try {
            int n = w2_readIntInline(s);
            for (int i = 0; i < n; ++i) {
                obj = new QuestDetails(m_version);
                static_cast<Streamable*>(obj)->read(s, tag);
                this->add(obj);
            }
            return;
        } catch (...) {
            if (obj)
                delete static_cast<Streamable*>(obj);
        }
        ++idx;
    }
}

void QuestsDetails::writeList(oStream& s, int tag)
{
    s << (unsigned long)m_count;
    for (int i = 0; i < m_count; ++i) {
        QuestDetails* q = m_items[i];
        static_cast<Streamable*>(q)->write(s, tag);
    }
}
