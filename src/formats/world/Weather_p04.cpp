#include "Weather.h"
#include "p04_common.h"
#include "Strings.h"
#include "String.hpp"
#include "iStream.h"
#include "oStream.h"
#include "Name.h"

void Weather::clear()
{
    Strings& strings = FIELD(Strings, this, 0x20);
    FIELD(float, this, 0x14) = 0;
    FIELD(int, this, 0x18) = -1;
    FIELD(int, this, 0x1C) = 0;
    strings.deleteAll();
    strings.m_ids.m_count = 0;
}

void Weather::read(iStream& in, DWORD fmt)
{
    Strings& strings = FIELD(Strings, this, 0x20);
    FIELD(float, this, 0x14) = in.readFloat();
    FIELD(int, this, 0x18) = (int)in.readInt();
    FIELD(int, this, 0x1C) = (int)in.readInt();
    int count = (int)in.readInt();
    String name;
    for (int i = 0; i < count; ++i)
    {
        readFixedString256(in, name, 0);
        strings.add((long)i, name.m_buffer);
    }
}

void Weather::write(oStream& out, DWORD fmt) const
{
    Strings& strings = FIELD(Strings, this, 0x20);
    out << FIELD(float, this, 0x14);
    out << FIELD(int, this, 0x18);
    out << FIELD(int, this, 0x1C);
    int count = strings.size();
    out << count;
    for (int i = 0; i < count; ++i)
    {
        const wchar_t* text;
        String* item;
        if (i >= 0 && i < strings.m_count && (item = strings.m_items[i]) != 0)
            text = item->m_buffer;
        else
            text = 0;
        String tmp;
        if (text)
            tmp.append(text, (unsigned)wcslen(text));
        writeFixedString256(out, tmp, 0);
    }
}

bool Weather::toText(String& out, DWORD fmt) const
{
    Strings& strings = FIELD(Strings, this, 0x20);
    int count = strings.size();
    for (int i = 0; i < count; ++i)
    {
        const wchar_t* text;
        String* item;
        if (i >= 0 && i < strings.m_count && (item = strings.m_items[i]) != 0)
            text = item->m_buffer;
        else
            text = 0;
        out << text;
        out.newline();
    }
    return true;
}
