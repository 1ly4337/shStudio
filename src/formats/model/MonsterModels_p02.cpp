#include "p02_common.h"
#include "MonsterModels.h"
#include "MonsterModel.h"
#include "ItemModels.h"
#include "ItemModel.h"
#include "ClothTextures.h"
#include "Strings.h"

void MonsterModels::read(iStream& in, DWORD fmt)
{
    deleteAll();
    MonsterModel* current = 0;
    try
    {
        m_signature = 0x20000000;
        in.read(&m_signature, 3);
        DWORD v = (DWORD)m_signature;
        m_signature = (int)((v >> 24) | ((v >> 8) & 0xFF00) | ((v << 8) & 0xFF0000) | (v << 24));
        if (m_signature != 'MO2 ' && m_signature != 'MO4 ')
            throw;
        int n = in.readInt();
        for (int i = 0; i < n; ++i)
        {
            MonsterModel* m = new MonsterModel((DWORD)m_signature, i, 0);
            current = m;
            m->read(in, fmt);
            add(m);
        }
    }
    catch (...)
    {
        delete current;
        deleteAll();
        throw;
    }
}

void MonsterModels::write(oStream& out, DWORD fmt) const
{
    out.m_writeNul = false;
    if (m_signature == 'MO4 ')
        out.put("MO4", 3);
    else
        out.put("MO2", 3);
    out << (unsigned long)m_count;
    for (int i = 0; i < m_count; ++i)
        m_items[i]->write(out, fmt);
}

bool ClothTextures::isEmpty() const
{
    return m_names.size() == 0;
}

void ClothTextures::clear()
{
    m_names.deleteAll();
    m_names.m_ids.m_count = 0;
}

void ClothTextures::read(iStream& in, DWORD )
{
    int recordSize = 0;
    if (m_value == 1)
        recordSize = 256;
    else if (m_value == 2)
        recordSize = 260;
    try
    {
        int n = in.readInt();
        if (m_value == 0)
        {
            recordSize = (in.m_size - in.m_pos) / n;
            if (recordSize == 256)
                m_value = 1;
            else if (recordSize == 260)
                m_value = 2;
            else
                throw 0;
        }
        for (int i = 0; i < n; ++i)
        {
            char buffer[264];
            in.gets(buffer, recordSize);
            String name;
            int len = (int)strlen(buffer);
            if (name.reserve(len))
            {
                for (int k = 0; k < len; ++k)
                    name.m_buffer[name.m_length + k] = (wchar_t)(short)buffer[k];
                name.m_length += len;
            }
            m_names.add(name.m_buffer);
        }
    }
    catch (...)
    {
        clear();
        throw;
    }
}

void ClothTextures::write(oStream& out, DWORD ) const
{
    int recordSize = 4 * (m_value != 1) + 256;
    int n = m_names.size();
    out << (unsigned long)n;
    for (int i = 0; i < n; ++i)
    {
        char buffer[260];
        memset(buffer, 0, recordSize);
        if (i >= 0 && i < m_names.m_count)
        {
            const String* s = m_names.m_items[i];
            const wchar_t* text = s ? s->m_buffer : 0;
            if (text)
            {
                int len = (int)wcslen(text);
                for (int j = 0; j < len; ++j)
                    buffer[j] = (char)text[j];
            }
        }
        out.put(buffer, recordSize);
    }
}
