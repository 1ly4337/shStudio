#include "Monsters.h"
#include "Monster.h"
#include "String.hpp"
#include "iStream.hpp"
#include "oStream.hpp"
#include "_imports_w0.h"
#include "p03_free.h"

HRESULT Monsters::setCodePage(int format)
{
    m_format = format;
    for (int i = 0; i < m_count; ++i)
        m_items[i]->m_version = format;
    return 0;
}

void Monsters::setExtended(bool extended)
{
    m_extended = extended;
    for (int i = 0; i < m_count; ++i)
        m_items[i]->m_extended = extended;
}

Monster* Monsters::findById(int id)
{
    int count = m_count;
    int i = 0;
    if (count <= 0)
        return 0;
    for (Monster** p = m_items; (*p)->m_id != id; ++p)
    {
        if (++i >= count)
            return 0;
    }
    return m_items[i];
}

void Monsters::readHeader(iStream& s)
{
    Serializable::checkSignature(s, L"id");
    Serializable::checkSignature(s, L"image");
    Serializable::checkSignature(s, L"level");
    Serializable::checkSignature(s, L"exp");
    Serializable::checkSignature(s, L"ai");
    for (int i = 1; i <= 2; ++i)
    {
        String col = sub_43CCF0(L"money", i);
        Serializable::checkSignature(s, col.m_buffer);
    }
    for (int j = 1; j <= 9; ++j)
    {
        {
            String col = sub_43CCF0(L"item", j);
            Serializable::checkSignature(s, col.m_buffer);
        }
        String col = sub_43CCF0(L"itemdroprate", j);
        Serializable::checkSignature(s, col.m_buffer);
    }
    Serializable::checkSignature(s, L"questitem");
    Serializable::checkSignature(s, L"hp");
    Serializable::checkSignature(s, L"sp");
    Serializable::checkSignature(s, L"mp");
    Serializable::checkSignature(s, L"dex");
    Serializable::checkSignature(s, L"wis");
    Serializable::checkSignature(s, L"luc");
    Serializable::checkSignature(s, L"day");
    Serializable::checkSignature(s, L"size");
    Serializable::checkSignature(s, L"attrib");
    Serializable::checkSignature(s, L"defense");
    Serializable::checkSignature(s, L"magic");
    for (int k = 1; k <= 15; ++k)
    {
        String col = sub_43CCF0(L"state", k);
        Serializable::checkSignature(s, col.m_buffer);
    }
    for (int m = 1; m <= 6; ++m)
    {
        String col = sub_43CCF0(L"skill", m);
        Serializable::checkSignature(s, col.m_buffer);
    }
    Serializable::checkSignature(s, L"normaltime");
    Serializable::checkSignature(s, L"normalstep");
    Serializable::checkSignature(s, L"chasetime");
    Serializable::checkSignature(s, L"chasestep");
    Serializable::checkSignature(s, L"chaserange");
    static const wchar_t* const attackCols[9] = {
        L"attackani", L"attacktype", L"attacktime", L"attackrange", L"attack",
        L"attackplus", L"attackattrib", L"attackspecial", L"attackok" };
    for (int n = 1; n <= 3; ++n)
    {
        for (int c = 0; c < 9; ++c)
        {
            String col = sub_43CCF0(attackCols[c], n);
            Serializable::checkSignature(s, col.m_buffer);
        }
    }
    Serializable::checkSignature(s, L"colortype");
    Serializable::checkSignature(s, L"colorhue");
    Serializable::checkSignature(s, L"colorsaturation");
    Serializable::checkSignature(s, L"colorlight");
}

static void writeColumnName(oStream& s, const wchar_t* name)
{
    int len = name ? (int)wcslen(name) : 0;
    unsigned char b = (unsigned char)len;
    s.put(&b, 1);
    if (name)
        s.put(name, 2 * len);
}

void Monsters::writeHeader(oStream& s)
{
    writeColumnName(s, L"id");
    writeColumnName(s, L"image");
    writeColumnName(s, L"level");
    writeColumnName(s, L"exp");
    writeColumnName(s, L"ai");
    for (int i = 1; i <= 2; ++i)
    {
        String col = sub_43CCF0(L"money", i);
        writeColumnName(s, col.m_buffer);
    }
    for (int j = 1; j <= 9; ++j)
    {
        {
            String col = sub_43CCF0(L"item", j);
            writeColumnName(s, col.m_buffer);
        }
        String col = sub_43CCF0(L"itemdroprate", j);
        writeColumnName(s, col.m_buffer);
    }
    writeColumnName(s, L"questitem");
    writeColumnName(s, L"hp");
    writeColumnName(s, L"sp");
    writeColumnName(s, L"mp");
    writeColumnName(s, L"dex");
    writeColumnName(s, L"wis");
    writeColumnName(s, L"luc");
    writeColumnName(s, L"day");
    writeColumnName(s, L"size");
    writeColumnName(s, L"attrib");
    writeColumnName(s, L"defense");
    writeColumnName(s, L"magic");
    for (int k = 1; k <= 15; ++k)
    {
        String col = sub_43CCF0(L"state", k);
        writeColumnName(s, col.m_buffer);
    }
    for (int m = 1; m <= 6; ++m)
    {
        String col = sub_43CCF0(L"skill", m);
        writeColumnName(s, col.m_buffer);
    }
    writeColumnName(s, L"normaltime");
    writeColumnName(s, L"normalstep");
    writeColumnName(s, L"chasetime");
    writeColumnName(s, L"chasestep");
    writeColumnName(s, L"chaserange");
    static const wchar_t* const attackCols[9] = {
        L"attackani", L"attacktype", L"attacktime", L"attackrange", L"attack",
        L"attackplus", L"attackattrib", L"attackspecial", L"attackok" };
    for (int n = 1; n <= 3; ++n)
    {
        for (int c = 0; c < 9; ++c)
        {
            String col = sub_43CCF0(attackCols[c], n);
            writeColumnName(s, col.m_buffer);
        }
    }
    writeColumnName(s, L"colortype");
    writeColumnName(s, L"colorhue");
    writeColumnName(s, L"colorsaturation");
    writeColumnName(s, L"colorlight");
}

void Monsters::read(iStream& s, DWORD tag)
{
    if (tag == 0x6C747874 )
    {
        if (m_buffer2)
        {
            operator delete[](m_buffer2);
            m_buffer2 = 0;
        }
        m_buffer2 = (wchar_t*)s.readAlloc(0x80);
        if (s.readDword() != 2)
            throw L"Invalid or not supported DB format";
        Serializable::checkSignature(s, L"id");
        Serializable::checkSignature(s, L"name");
        int n = s.readInt();
        String skip;
        if (n > 0)
        {
            do
            {
                int id = s.readInt64();
                Monster* m = findById(id);
                if (m)
                    m->read(s, 0x6C747874 );
                else
                    skip.read(s, 4, (UINT)-1);
                --n;
            } while (n);
        }
        return;
    }

    if (m_buffer1)
    {
        operator delete[](m_buffer1);
        m_buffer1 = 0;
    }
    deleteAll();

    if (tag == 0x666C6E74 )
    {
        m_buffer1 = (wchar_t*)s.readAlloc(0x80);
        if (s.readDword() != 94)
            throw L"Invalid or not supported DB format";
        m_format = 0x1600;
        readHeader(s);
        int n = s.readDword();
        if (n > 0)
        {
            do
            {
                int id = s.readInt64();
                Monster* m = new Monster(id, 0x1620);
                add(m);
                m->read(s, 0x666C6E74 );
                --n;
            } while (n);
        }
    }
    else if (tag == 0x66696C65 )
    {
        int versions[3] = { 0x620, 0x500, 0 };
        for (int vi = 0; ; ++vi)
        {
            deleteAll();
            s.m_pos = 0;
            m_format = versions[vi];
            if (!versions[vi])
                throw 1;
            try
            {
                int n = s.readInt();
                if (!n && s.m_size - s.m_pos > 0)
                {
                    s.m_pos = 0;
                    s.skip(64);
                    n = s.readInt();
                }
                for (int i = 0; i < n; ++i)
                {
                    Monster* m = new Monster(i + 1, m_format);
                    add(m);
                    m->m_extended = m_extended;
                    m->read(s, tag);
                }
                return;
            }
            catch (...)
            {
            }
        }
    }
}

void Monsters::write(oStream& s, DWORD tag) const
{
    sortList(*const_cast<Monsters*>(this), 0x6D626964 , 0, true);
    if (tag == 0x6C747874 )
    {
        if (!m_buffer2)
            throw L"Error: textHeader does not exist";
        s.put(m_buffer2, 128);
        (s << (unsigned long)(2));
        s << L"id";
        s << L"name";
    }
    else if (tag == 0x666C6E74 )
    {
        if (!m_buffer1)
            throw "Error: dataHeader does not exist";
        s.put(m_buffer1, 128);
        (s << (unsigned long)(94));
        writeHeader(s);
    }
    (s << (unsigned long)(m_count));
    for (int i = 0; i < m_count; ++i)
        m_items[i]->write(s, tag);
}

void sqlTruncateHeader(String& out, const wchar_t* table1, const wchar_t* table2)
{
    String& a = (out << (const wchar_t*)(L"USE [PS_GameDefs]"));
    String& b = (a << (const wchar_t*)(L"\r\n"));
    String& c = (b << (const wchar_t*)(L"GO"));
    String& d = (c << (const wchar_t*)(L"\r\n"));
    String& e = (d << (const wchar_t*)(L"TRUNCATE TABLE ["));
    String& f = (e << (const wchar_t*)(table1));
    (f << (char)(']'));
    (f << (const wchar_t*)(L"\r\n"));
    if (table2)
    {
        String& g = (out << (const wchar_t*)(L"TRUNCATE TABLE ["));
        String& h = (g << (const wchar_t*)(table2));
        (h << (char)(']'));
        (h << (const wchar_t*)(L"\r\n"));
    }
    String& i = (out << (const wchar_t*)(L"GO"));
    (i << (const wchar_t*)(L"\r\n"));
    out.m_flags |= 2;
}

bool Monsters::toText(String& out, DWORD tag) const
{
    bool ok = true;
    switch (tag)
    {
    case 0x6D6F6273 :
        sqlTruncateHeader(out, L"Mobs", 0);
        for (int i = 0; i < m_count && ok; ++i)
            ok = m_items[i]->toText(out, 0x6D6F6273 );
        break;
    case 0x64726F70 :
        sqlTruncateHeader(out, L"MobItems", 0);
        for (int i = 0; i < m_count && ok; ++i)
            ok = m_items[i]->toText(out, 0x64726F70 );
        break;
    case 0x6D626470 :
        sqlTruncateHeader(out, L"Mobs", L"MobItems");
        for (int i = 0; i < m_count && ok; ++i)
            ok = m_items[i]->toText(out, 0x6D6F6273 );
        (out << (const wchar_t*)(L"\r\n"));
        for (int i = 0; i < m_count && ok; ++i)
            ok = m_items[i]->toText(out, 0x64726F70 );
        break;
    default:
        for (int j = 0; j < m_count && ok; ++j)
            ok = m_items[j]->toText(out, tag);
        break;
    }
    return ok;
}
