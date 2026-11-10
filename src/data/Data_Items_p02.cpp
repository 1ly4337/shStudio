#include "p02_common.h"
#include "Data_Items.h"
#include "Data_Item.h"
#include "Collection.hpp"
#include "FlntRows.h"

namespace Data {

Item* Items::sub_42FC00(int type, int typeId) const
{
    for (int i = 0; i < m_count; ++i)
    {
        Item* item = m_items[i];
        if (item->m_type == type && item->m_typeId == typeId)
            return item;
    }
    return 0;
}

int Items::sub_42FC50(ptrArray<Item*>* out, int grade)
{
    for (int i = 0; i < out->m_count; ++i)
        out->m_items[i] = 0;
    out->m_count = 0;
    for (int i = 0; i < m_count; ++i)
    {
        Item* item = m_items[i];
        if (item && item->m_grade == grade)
            out->add(item);
    }
    return out->size();
}

bool Items::sub_42FCC0(int mode, BYTE kind, bool flag, int slot, ptrArray<Item*>* out)
{
    for (int i = 0; i < out->m_count; ++i)
        out->m_items[i] = 0;
    out->m_count = 0;
    if (!m_count)
        return false;
    for (int i = 0; i < m_count; ++i)
    {
        Item* it = m_items[i];
        BYTE t = it->m_type;
        if (it->m_name.m_buffer && it->m_name.m_length && it->m_name.m_buffer[0] == L'?')
            continue;
        if (mode == 2)
        {
            if (!it->m_level || (slot != -1 && slot != it->m_slot))
                continue;
            if (flag)
            {
                if (t == 19 || t == 69 || t == 75)
                {
                    out->add(it);
                    continue;
                }
            }
            else if (t == 34 || t == 84 || t == 90)
            {
                out->add(it);
                continue;
            }
            if ((!kind && Item::isGearType(t, flag)) || Item::matchesType(it, kind))
                out->add(it);
            continue;
        }
        if (mode == 1)
        {
            if (!it->m_level || it->m_country != 2 || !flag)
            {
                BYTE c = it->m_country;
                if ((c != 5 || flag) && c != 6)
                    continue;
            }
            bool ok;
            switch (kind)
            {
            case 1:  if (t == 1) goto matched;  ok = t == 45; break;
            case 2:  if (t == 2) goto matched;  ok = t == 46; break;
            case 3:  if (t == 3) goto matched;  ok = t == 47; break;
            case 4:  if (t == 4) goto matched;  ok = t == 48; break;
            case 5:  if (t == 5) goto matched;  ok = t == (flag == 0) + 49; break;
            case 6:  if (t == 6) goto matched;  ok = t == (flag == 0) + 51; break;
            case 7:  if (t == 7) goto matched;  ok = t == (flag == 0) + 53; break;
            case 8:  if (t == 8) goto matched;  ok = t == (flag == 0) + 55; break;
            case 9:  if (t == 9) goto matched;  ok = t == 57; break;
            case 10: if (t == 10) goto matched; ok = t == 58; break;
            case 11: if (t == 11) goto matched; ok = t == 59; break;
            case 12: if (t == 12) goto matched; ok = t == (flag == 0) + 60; break;
            case 13: if (t == 13) goto matched; ok = t == (flag == 0) + 62; break;
            case 14: if (t == 14) goto matched; ok = t == 64; break;
            case 15: if (t == 15) goto matched; ok = t == 65; break;
            default: continue;
            }
            if (!ok)
                continue;
        matched:
            if (slot == -1 || it->m_slot == slot)
                out->add(it);
            continue;
        }
        if (mode == 3)
        {
            switch (kind)
            {
            case 0:
                if (t == 22 || t == 23 || t == 37 || t == 40 || t == 96 || t == 97)
                    out->add(it);
                break;
            case 0x16:
                if (t == 22 || t == 37)
                    out->add(it);
                break;
            case 0x17:
                if (t == 23 || t == 96)
                    out->add(it);
                break;
            case 0x19:
                if (t == 25)
                    out->add(it);
                break;
            case 0x1E:
                if (t == 30 || t == 95)
                    out->add(it);
                break;
            case 0x28:
                if (t == 40 || t == 97)
                    out->add(it);
                break;
            default:
                if (t == kind)
                    out->add(it);
                break;
            }
            continue;
        }
        if (mode == 10)
        {
            if (t == kind)
                out->add(it);
            continue;
        }
        if (mode != 4)
        {
            switch (mode)
            {
            case 5:
                if (t != 42)
                    break;
                if (flag)
                {
                    if (it->m_country < 2u)
                        out->add(it);
                    break;
                }
                if (it->m_country == 3 || it->m_country == 4)
                    out->add(it);
                break;
            case 6:
                if (it->m_level <= 0)
                    break;
                if (flag)
                {
                    if (t == 24)
                        out->add(it);
                    break;
                }
                if (t == 39)
                    out->add(it);
                break;
            case 7:
                if (t == 94)
                    out->add(it);
                break;
            case 8:
                if (t == 43)
                    out->add(it);
                break;
            case 9:
                if (t == 78 || t == 79 || t == 80)
                    out->add(it);
                break;
            default:
                break;
            }
            continue;
        }
        if (t == 25)
        {
            if (!it->m_hp && !it->m_mp && !it->m_sp && !it->m_range)
            {
                BYTE s = it->m_special;
                if (!s || s == 42)
                    continue;
            }
            out->add(it);
            continue;
        }
        if (t != 44)
        {
            if (t != 100)
                continue;
            WORD r = (WORD)it->m_range;
            if (r < 280 || r > 300)
                continue;
            out->add(it);
            continue;
        }
        if (it->m_range)
            out->add(it);
    }
    return true;
}

bool Items::sub_4301C0(iStream& in, DWORD fmt, int version)
{
    in.m_pos = 0;
    deleteAll();
    m_value30 = version;
    int index = 1;
    try
    {
        m_unk2C = in.readInt();
        for (int type = 1; type <= m_unk2C; ++type)
        {
            int n = in.readInt();
            for (int id = 1; id <= n; ++id)
            {
                Item* item = new Item(index, m_value30, index);
                ++index;
                try
                {
                    item->read(in, fmt);
                }
                catch (...)
                {
                    throw;
                }
                if (!item->m_type)
                {
                    item->m_type = (BYTE)type;
                    item->m_typeId = (BYTE)id;
                    item->m_name = L"???";
                    item->m_name << (long)id;
                }
                add(item);
            }
        }
        if (m_count >= 2)
        {
            m_changes = 0;
            quickSort(*static_cast<ptrArray<Item*>*>(this), 0, m_count - 1, 'type', 'tpid', true);
        }
        return true;
    }
    catch (...)
    {
        deleteAll();
        return false;
    }
}

HRESULT Items::setCodePage(int version)
{
    m_value30 = version;
    for (int i = 0; i < m_count; ++i)
        m_items[i]->m_version = version;
    return S_OK;
}

void Items::sub_4303E0(iStream& in)
{
    static const wchar_t* const columns[] = {
        L"itemtype", L"itemtypeid", L"image", L"icon", L"level", L"country", L"attackfighter", L"defensefighter",
        L"patrolrogue", L"shootrogue", L"attackmage", L"defensemage", L"grow", L"str", L"dex", L"rec", L"int",
        L"wis", L"luc", L"vg", L"og", L"ig", L"range", L"attacktime", L"attrib", L"special", L"slot", L"quality",
        L"effect1", L"effect2", L"effect3", L"effect4", L"consthp", L"constsp", L"constmp", L"conststr",
        L"constdex", L"constrec", L"constint", L"constwis", L"constluc", L"speed", L"exp", L"buy", L"sell",
        L"grade", L"drop", L"server", L"count", L"duration", L"extduration", L"secoption", L"optionrate",
        L"buymethod", L"maxlevel", L"weaponpart", L"dyeingtype", L"arg3", L"arg4", L"usecontype", L"useconvar",
        L"moneytype", L"itemskill", L"itemupgrade", L"arg10", L"genecount", L"arg12", L"spellbookexp",
        L"spellbookdurability"
    };
    for (int i = 0; i < 69; ++i)
        Serializable::checkSignature(in, columns[i]);
}

void Items::sub_430740(oStream& out)
{
    out << L"itemtype" << L"itemtypeid" << L"image" << L"icon" << L"level" << L"country" << L"attackfighter"
        << L"defensefighter" << L"patrolrogue" << L"shootrogue" << L"attackmage" << L"defensemage" << L"grow"
        << L"str" << L"dex" << L"rec" << L"int" << L"wis" << L"luc" << L"vg" << L"og" << L"ig" << L"range"
        << L"attacktime" << L"attrib" << L"special" << L"slot" << L"quality" << L"effect1" << L"effect2"
        << L"effect3" << L"effect4" << L"consthp" << L"constsp" << L"constmp" << L"conststr" << L"constdex"
        << L"constrec" << L"constint" << L"constwis" << L"constluc" << L"speed" << L"exp" << L"buy" << L"sell"
        << L"grade" << L"drop" << L"server" << L"count" << L"duration" << L"extduration" << L"secoption"
        << L"optionrate" << L"buymethod" << L"maxlevel" << L"weaponpart" << L"dyeingtype" << L"arg3" << L"arg4"
        << L"usecontype" << L"useconvar" << L"moneytype" << L"itemskill" << L"itemupgrade" << L"arg10"
        << L"genecount" << L"arg12" << L"spellbookexp" << L"spellbookdurability";
}

void Items::read(iStream& in, DWORD fmt)
{
    if (fmt == 'ltxt')
    {
        if (m_buffer2)
        {
            delete[] m_buffer2;
            m_buffer2 = 0;
        }
        m_buffer2 = (wchar_t*)in.readAlloc(128);
        if (in.readDword() != 4)
            throw L"Invalid or not supported DB format";
        {
            static const wchar_t* const names[4] = { L"itemtype", L"itemtypeid", L"itemname", L"text" };
            int start = static_cast<aStream&>(in).m_pos;
            bool seen[4] = { false, false, false, false };
            for (int c = 0; c < 4; ++c)
            {
                String name;
                name.read(in, 1, 1200);
                int k = 0;
                while (k < 4 && (seen[k] || !name.equalsNoCase(names[k])))
                    ++k;
                if (k == 4)
                    throw (const wchar_t*)L"Invalid or not supported DB format";
                seen[k] = true;
            }
            aStream& s = static_cast<aStream&>(in);
            FlntRows::rememberColumns(this, s.m_data + start, s.m_pos - start);
        }
        int n = in.readInt();
        String skip;
        if (n > 0)
        {
            do
            {
                int type = (int)in.readInt64();
                int typeId = (int)in.readInt64();
                Item* found = 0;
                for (int i = 0; i < m_count; ++i)
                    if (m_items[i]->m_type == type && m_items[i]->m_typeId == typeId)
                    {
                        found = m_items[i];
                        break;
                    }
                if (found)
                    found->read(in, 'ltxt');
                else
                {
                    skip.read(in, 4, (UINT)-1);
                    skip.read(in, 4, (UINT)-1);
                }
            } while (n-- != 1);
        }
        return;
    }
    if (m_buffer1)
    {
        delete[] m_buffer1;
        m_buffer1 = 0;
    }
    deleteAll();
    if (fmt == 'flnt')
    {
        m_buffer1 = (wchar_t*)in.readAlloc(128);
        int columns = in.readDword();
        if (columns < 69)
            throw L"Invalid or not supported DB format";
        m_value30 = 0x1600;
        Data::Items::sub_4303E0(in);
        String extra;
        for (int k = columns - 69; k > 0; --k)
            extra.read(in, 1, 1200);
        int n = in.readDword();
        for (int i = 1; i <= n; ++i)
        {
            Item* item = new Item(i, 0x1620, 0);
            add(item);
            item->read(in, 'flnt');
            for (int k = columns - 69; k > 0; --k)
                in.readInt64();
        }
    }
    else if (fmt == 'file' && !Data::Items::sub_4301C0(in, 'file', 0x500) && !Data::Items::sub_4301C0(in, 'file', 0x600)
             && !Data::Items::sub_4301C0(in, 'file', 0x620))
    {
        throw (long)E_FAIL;
    }
}

void Items::write(oStream& out, DWORD fmt) const
{
    Items* self = const_cast<Items*>(this);
    if (m_count >= 2)
    {
        self->m_changes = 0;
        quickSort(*static_cast<ptrArray<Item*>*>(self), 0, m_count - 1, 'type', 'tpid', true);
    }
    switch (fmt)
    {
    case 'ltxt':
        if (!m_buffer2)
            throw "Error: textHeader does not exist";
        out.put(m_buffer2, 128);
        out << (unsigned long)4;
        {
            int columnsSize = 0;
            if (const BYTE* columns = FlntRows::columns(this, &columnsSize))
                out.put(columns, columnsSize);
            else
            {
                out << L"itemtype";
                out << L"itemtypeid";
                out << L"itemname";
                out << L"text";
            }
        }
        out << (unsigned long)m_count;
        for (int i = 0; i < m_count; ++i)
            m_items[i]->write(out, 'ltxt');
        break;
    case 'flnt':
        if (!m_buffer1)
            throw "Error: dataHeader does not exist";
        out.put(m_buffer1, 128);
        out << (unsigned long)69;
        const_cast<Items*>(this)->Data::Items::sub_430740(out);
        out << (unsigned long)m_count;
        for (int i = 0; i < m_count; ++i)
            m_items[i]->write(out, 'flnt');
        break;
    case 'file':
    {
        int maxType = 0;
        for (int i = 0; i < m_count; ++i)
            if (maxType < m_items[i]->m_type)
                maxType = m_items[i]->m_type;
        self->m_unk2C = maxType;
        out << (unsigned long)maxType;
        int index = 0;
        for (int type = 1; type <= m_unk2C; ++type)
        {
            int n = 0;
            for (int i = 0; i < m_count; ++i)
                if (m_items[i]->m_type == type)
                    ++n;
            out << (unsigned long)n;
            for (; n > 0; --n)
                m_items[index++]->write(out, 'file');
        }
        break;
    }
    }
}

bool Items::toText(String& out, DWORD fmt) const
{
    if (fmt == 'sql ')
    {
        out << L"TRUNCATE TABLE [PS_GameDefs].[dbo].[Items];" << L"\r\n";
        for (int i = 0; i < m_count; ++i)
        {
            if (!(i % 100))
                out << L"GO" << L"\r\n";
            m_items[i]->toText(out, 'sql ');
        }
        out << L"GO";
        out.newline();
        return true;
    }
    for (int i = 0; i < m_count; ++i)
        m_items[i]->toText(out, fmt);
    return true;
}

}
