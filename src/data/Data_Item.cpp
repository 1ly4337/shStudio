#include "FlntRows.h"
#include "Data_Item.h"
#include "ClientData.h"

const wchar_t* g_itemColumnNames[54] = {
    L"Type", L"TypeID", L"Name", L"Description", L"Model", L"Icon", L"Level", L"Country",
    L"Class1", L"Class2", L"Class3", L"Class4", L"Class5", L"Class6", L"Grow", L"Type2", L"Type3",
    L"ReqStr", L"ReqDex", L"ReqRec", L"ReqInt", L"ReqWis", L"ReqLuc", L"ReqVg", L"ReqOg", L"ReqIg",
    L"Range", L"AttackTime", L"Attrib", L"Special", L"Slot", L"Quality", L"Attack", L"Atk Add", L"Def",
    L"Resist", L"Str", L"Dex", L"Rec", L"Int", L"Wis", L"Luc", L"HP", L"SP", L"MP", L"Speed", L"Exp",
    L"Buy", L"Sell", L"Grade", L"Drop", L"Server", L"Count", 0
};
const wchar_t* g_itemColumnNamesEp6[21] = {
    L"Duration", L"Ext. Duration", L"Sec. Option", L"Option Rate", L"Buy Method", L"Max Level", L"Weapon Part",
    L"Dyeing Type", L"Arg. 3", L"Arg. 4", L"UseConType", L"UseConVar", L"Money Type", L"Item Skill", L"Item Upgrade",
    L"Arg. 10", L"Gene Count", L"Arg. 12", L"Spell Exp", L"Spell Duration", 0
};
DWORD g_itemColumnTagsEp6[21] = {
    'dura', 'edur', 'sopt', 'orat', 'bmth', 'mxlv', 'wprt', 'dyty', 'arg3', 'arg4',
    'ucty', 'ucvr', 'mnty', 'itsk', 'itup', 'argA', 'gnct', 'argC', 'spex', 'spdu', 0
};
DWORD g_itemColumnTags[54] = {
    'type', 'tpid', 'name', 'desc', 'mdel', 'icon', 'lvl ', 'cntr', 'cla1', 'cla2', 'cla3', 'cla4',
    'cla5', 'cla6', 'grow', 'typ2', 'typ3', 'rStr', 'rDex', 'rRec', 'rInt', 'rWis', 'rLuc', 'rVg ',
    'rOg ', 'rIg ', 'rang', 'atkt', 'attr', 'spec', 'slot', 'qual', 'attk', 'atpl', 'defp', 'resm',
    'cStr', 'cDex', 'cRec', 'cInt', 'cWis', 'cLuc', 'cHP ', 'cSP ', 'cMP ', 'sped', 'exp ', 'buy ',
    'sell', 'grde', 'drop', 'srvr', 'cnt ', 0
};
wchar_t g_itemTypeText[16];

namespace Data {

template <class T>
static inline int cmp3(T a, T b)
{
    if (a == b)
        return 0;
    return a < b ? -1 : 1;
}

Item::Item(int index, int version, int )
{
    m_index = index;
    m_version = version;
    m_type = 0;
    m_typeId = 0;
    reset();
}

Item::~Item()
{
    FlntRows::forget(this);
}

int Item::getVersion() const
{
    return 514;
}

bool Item::copyFrom(const Streamable* src)
{
    const Item* p = dynamic_cast<const Item*>(src);
    if (p)
        return assign(p);
    return false;
}

int Item::nodeType() const
{
    return 'item';
}

int Item::getId() const
{
    return itemIconBase(m_type) + m_icon;
}

const wchar_t* Item::getString(DWORD tag) const
{
    switch (tag)
    {
    case 'name':
        return m_name.m_buffer;
    case 'desc':
        return m_desc.m_buffer;
    case 'cnts':
        if (m_type == 30)
            return L"All";
        switch (m_country)
        {
        case 0: return L"Hum";
        case 1: return L"Elf";
        case 2: return L"Light";
        case 3: return L"Dth";
        case 4: return L"Vail";
        case 5: return L"Dark";
        case 6: return L"All";
        default: return L"--";
        }
    case 'typS':
        swprintf_s(g_itemTypeText, 16, L"%i-%i", m_type, m_typeId);
        return g_itemTypeText;
    case 'dtls':
    {
        String& d = const_cast<String&>(m_details);
        d.clear();
        if (m_str)
            (d << L", Str:") << (unsigned short)m_str;
        if (m_dex)
            (d << L", Dex:") << (unsigned short)m_dex;
        if (m_rec)
            (d << L", Rec:") << (unsigned short)m_rec;
        if (m_int)
            (d << L", Int:") << (unsigned short)m_int;
        if (m_wis)
            (d << L", Wis:") << (unsigned short)m_wis;
        if (m_luc)
            (d << L", Luc:") << (unsigned short)m_luc;
        if (m_hp)
            (d << L", HP:") << (unsigned short)m_hp;
        if (m_sp)
            (d << L", SP:") << (unsigned short)m_sp;
        if (m_mp)
            (d << L", MP:") << (unsigned short)m_mp;
        if (d.m_length)
            d = d.right(d.m_length - 2);
        return d.m_buffer;
    }
    default:
        return 0;
    }
}

int Item::getInt(DWORD tag) const
{
    switch (tag)
    {
    case 'type': return m_type;
    case 'tpid': return m_typeId;
    case 'mdel': return m_model;
    case 'icon': return m_icon;
    case 'lvl ': return m_level;
    case 'cntr': return m_country;
    case 'cla1': return m_class[0];
    case 'cla2': return m_class[1];
    case 'cla3': return m_class[2];
    case 'cla4': return m_class[3];
    case 'cla5': return m_class[4];
    case 'cla6': return m_class[5];
    case 'grow': return m_grow;
    case 'typ2': return m_type2;
    case 'typ3': return m_type3;
    case 'rStr': return m_reqStr;
    case 'rDex': return m_reqDex;
    case 'rRec': return m_reqRec;
    case 'rInt': return m_reqInt;
    case 'rWis': return m_reqWis;
    case 'rLuc': return m_reqLuc;
    case 'rVg ': return m_reqVg;
    case 'rOg ': return m_reqOg;
    case 'rIg ': return m_reqIg;
    case 'rang': return m_range;
    case 'atkt': return m_attackTime;
    case 'attr': return m_attrib;
    case 'spec': return m_special;
    case 'slot': return m_slot;
    case 'qual': return m_quality;
    case 'attk': return m_attack;
    case 'atpl': return m_attackAdd;
    case 'defp': return m_defense;
    case 'resm': return m_resist;
    case 'cHP ': return m_hp;
    case 'cSP ': return m_sp;
    case 'cMP ': return m_mp;
    case 'cStr': return m_str;
    case 'cDex': return m_dex;
    case 'cRec': return m_rec;
    case 'cInt': return m_int;
    case 'cWis': return m_wis;
    case 'cLuc': return m_luc;
    case 'sped': return m_speed;
    case 'exp ': return m_exp;
    case 'buy ': return m_buy;
    case 'sell': return m_sell;
    case 'grde': return m_grade;
    case 'drop': return m_drop;
    case 'srvr': return m_server;
    case 'cnt ': return m_count;
    case 'dura': return m_duration;
    case 'edur': return m_extDuration;
    case 'sopt': return m_secOption;
    case 'orat': return m_optionRate;
    case 'bmth': return m_buyMethod;
    case 'mxlv': return m_maxLevel;
    case 'wprt': return m_weaponPart;
    case 'dyty': return m_dyeingType;
    case 'arg3': return m_arg3;
    case 'arg4': return m_arg4;
    case 'ucty': return m_useConType;
    case 'ucvr': return m_useConVar;
    case 'mnty': return m_moneyType;
    case 'itsk': return m_itemSkill;
    case 'itup': return m_itemUpgrade;
    case 'argA': return m_arg10;
    case 'gnct': return m_geneCount;
    case 'argC': return m_arg12;
    case 'spex': return m_spellBookExp;
    case 'spdu': return m_spellBookDurability;
    }
    return 0;
}

bool Item::setInt(DWORD tag, int value)
{
    switch (tag)
    {
    case 'type': m_type = (BYTE)value; return true;
    case 'tpid': m_typeId = (BYTE)value; return true;
    case 'mdel': m_model = (BYTE)value; return true;
    case 'icon': m_icon = (BYTE)value; return true;
    case 'lvl ': m_level = (short)value; return true;
    case 'cntr': m_country = (BYTE)value; return true;
    case 'cla1': m_class[0] = (BYTE)value; return true;
    case 'cla2': m_class[1] = (BYTE)value; return true;
    case 'cla3': m_class[2] = (BYTE)value; return true;
    case 'cla4': m_class[3] = (BYTE)value; return true;
    case 'cla5': m_class[4] = (BYTE)value; return true;
    case 'cla6': m_class[5] = (BYTE)value; return true;
    case 'grow': m_grow = (BYTE)value; return true;
    case 'typ2': m_type2 = (BYTE)value; return true;
    case 'typ3': m_type3 = (BYTE)value; return true;
    case 'rStr': m_reqStr = (WORD)value; return true;
    case 'rDex': m_reqDex = (WORD)value; return true;
    case 'rRec': m_reqRec = (WORD)value; return true;
    case 'rInt': m_reqInt = (WORD)value; return true;
    case 'rWis': m_reqWis = (WORD)value; return true;
    case 'rLuc': m_reqLuc = (WORD)value; return true;
    case 'rVg ': m_reqVg = (short)value; return true;
    case 'rOg ': m_reqOg = (BYTE)value; return true;
    case 'rIg ': m_reqIg = (BYTE)value; return true;
    case 'rang': m_range = (short)value; return true;
    case 'atkt': m_attackTime = (BYTE)value; return true;
    case 'attr': m_attrib = (BYTE)value; return true;
    case 'spec': m_special = (BYTE)value; return true;
    case 'slot': m_slot = (BYTE)value; return true;
    case 'qual': m_quality = (WORD)value; return true;
    case 'attk': m_attack = (WORD)value; return true;
    case 'atpl': m_attackAdd = (WORD)value; return true;
    case 'defp': m_defense = (WORD)value; return true;
    case 'resm': m_resist = (WORD)value; return true;
    case 'cHP ': m_hp = (WORD)value; return true;
    case 'cSP ': m_sp = (WORD)value; return true;
    case 'cMP ': m_mp = (WORD)value; return true;
    case 'cStr': m_str = (WORD)value; return true;
    case 'cDex': m_dex = (WORD)value; return true;
    case 'cRec': m_rec = (WORD)value; return true;
    case 'cInt': m_int = (WORD)value; return true;
    case 'cWis': m_wis = (WORD)value; return true;
    case 'cLuc': m_luc = (WORD)value; return true;
    case 'sped': m_speed = (BYTE)value; return true;
    case 'exp ': m_exp = (BYTE)value; return true;
    case 'buy ': m_buy = (int)value; return true;
    case 'sell': m_sell = (int)value; return true;
    case 'grde': m_grade = (short)value; return true;
    case 'drop': m_drop = (short)value; return true;
    case 'srvr': m_server = (BYTE)value; return true;
    case 'cnt ': m_count = (BYTE)value; return true;
    case 'dura': m_duration = (int)value; return true;
    case 'edur': m_extDuration = (BYTE)value; return true;
    case 'sopt': m_secOption = (BYTE)value; return true;
    case 'orat': m_optionRate = (BYTE)value; return true;
    case 'bmth': m_buyMethod = (BYTE)value; return true;
    case 'mxlv': m_maxLevel = (BYTE)value; return true;
    case 'wprt': m_weaponPart = (BYTE)value; return true;
    case 'dyty': m_dyeingType = (BYTE)value; return true;
    case 'arg3': m_arg3 = (int)value; return true;
    case 'arg4': m_arg4 = (int)value; return true;
    case 'ucty': m_useConType = (int)value; return true;
    case 'ucvr': m_useConVar = (int)value; return true;
    case 'mnty': m_moneyType = (int)value; return true;
    case 'itsk': m_itemSkill = (int)value; return true;
    case 'itup': m_itemUpgrade = (int)value; return true;
    case 'argA': m_arg10 = (int)value; return true;
    case 'gnct': m_geneCount = (BYTE)value; return true;
    case 'argC': m_arg12 = (BYTE)value; return true;
    case 'spex': m_spellBookExp = (BYTE)value; return true;
    case 'spdu': m_spellBookDurability = (int)value; return true;
    default:
        return false;
    }
}

bool Item::setString(DWORD tag, const wchar_t* value)
{
    if (tag == 'name')
    {
        m_name = value;
        return true;
    }
    if (tag == 'desc')
    {
        m_desc = value;
        return true;
    }
    return false;
}

void Item::reset()
{
    m_name = L"???";
    if (m_typeId < 100)
        m_name << L'0';
    if (m_typeId < 10)
        m_name << L'0';
    m_name << (unsigned char)m_typeId;
    m_desc.clear();
    m_model = 0;
    m_icon = 0;
    m_level = 0;
    m_country = 0;
    for (int i = 0; i < 6; ++i)
        m_class[i] = 0;
    m_grow = 0;
    m_type3 = m_type;
    m_type2 = m_type;
    m_reqLuc = 0;
    m_reqVg = 0;
    m_reqInt = 0;
    m_reqWis = 0;
    m_reqDex = 0;
    m_reqRec = 0;
    m_reqStr = 0;
    m_unk72 = 0;
    m_reqOg = 0;
    m_reqIg = 0;
    m_special = 0;
    m_slot = 0;
    m_quality = 0;
    m_range = 0;
    m_attackTime = 0;
    m_attrib = 0;
    m_defense = 0;
    m_resist = 0;
    m_attack = 0;
    m_attackAdd = 0;
    m_sp = 0;
    m_mp = 0;
    m_hp = 0;
    m_wis = 0;
    m_luc = 0;
    m_rec = 0;
    m_int = 0;
    m_str = 0;
    m_dex = 0;
    m_speed = 0;
    m_exp = 0;
    m_sell = 0;
    m_buy = 0;
    m_grade = 0;
    m_drop = 0;
    m_server = 0;
    m_count = 0;
    m_duration = 0;
    m_extDuration = 0;
    m_secOption = 0;
    m_optionRate = 0;
    m_buyMethod = 0;
    m_weaponPart = 0;
    m_dyeingType = 0;
    m_maxLevel = 0;
    m_useConVar = 0;
    m_useConType = 0;
    m_arg4 = 0;
    m_arg3 = 0;
    m_arg10 = 0;
    m_itemUpgrade = 0;
    m_itemSkill = 0;
    m_moneyType = 0;
    m_arg12 = 0;
    m_spellBookExp = 0;
    m_geneCount = 0;
    m_spellBookDurability = 0;
    m_valid = false;
}

int Item::compare(const Node* other, DWORD tag) const
{
    const Item* p = dynamic_cast<const Item*>(other);
    switch (tag)
    {
    case 'name':
        return String::compareLogical(m_name, p->m_name);
    case 'desc':
        return String::compareLogical(m_desc, p->m_desc);
    case 'type': return cmp3(m_type, p->m_type);
    case 'tpid': return cmp3(m_typeId, p->m_typeId);
    case 'mdel': return cmp3(m_model, p->m_model);
    case 'icon': return cmp3(m_icon, p->m_icon);
    case 'lvl ': return cmp3(m_level, p->m_level);
    case 'cntr':
    case 'cnts': return cmp3(m_country, p->m_country);
    case 'cla1': return cmp3(m_class[0], p->m_class[0]);
    case 'cla2': return cmp3(m_class[1], p->m_class[1]);
    case 'cla3': return cmp3(m_class[2], p->m_class[2]);
    case 'cla4': return cmp3(m_class[3], p->m_class[3]);
    case 'cla5': return cmp3(m_class[4], p->m_class[4]);
    case 'cla6': return cmp3(m_class[5], p->m_class[5]);
    case 'grow': return cmp3(m_grow, p->m_grow);
    case 'typ2': return cmp3(m_type2, p->m_type2);
    case 'typ3': return cmp3(m_type3, p->m_type3);
    case 'rStr': return cmp3(m_reqStr, p->m_reqStr);
    case 'rDex': return cmp3(m_reqDex, p->m_reqDex);
    case 'rRec': return cmp3(m_reqRec, p->m_reqRec);
    case 'rInt': return cmp3(m_reqInt, p->m_reqInt);
    case 'rWis': return cmp3(m_reqWis, p->m_reqWis);
    case 'rLuc': return cmp3(m_reqLuc, p->m_reqLuc);
    case 'rVg ': return cmp3(m_reqVg, p->m_reqVg);
    case 'rOg ': return cmp3(m_reqOg, p->m_reqOg);
    case 'rIg ': return cmp3(m_reqIg, p->m_reqIg);
    case 'rang': return cmp3(m_range, p->m_range);
    case 'atkt': return cmp3(m_attackTime, p->m_attackTime);
    case 'attr': return cmp3(m_attrib, p->m_attrib);
    case 'spec': return cmp3(m_special, p->m_special);
    case 'slot': return cmp3(m_slot, p->m_slot);
    case 'qual': return cmp3(m_quality, p->m_quality);
    case 'attk': return cmp3(m_attack, p->m_attack);
    case 'atpl': return cmp3(m_attackAdd, p->m_attackAdd);
    case 'defp': return cmp3(m_defense, p->m_defense);
    case 'resm': return cmp3(m_resist, p->m_resist);
    case 'cHP ': return cmp3(m_hp, p->m_hp);
    case 'cSP ': return cmp3(m_sp, p->m_sp);
    case 'cMP ': return cmp3(m_mp, p->m_mp);
    case 'cStr': return cmp3(m_str, p->m_str);
    case 'cDex': return cmp3(m_dex, p->m_dex);
    case 'cRec': return cmp3(m_rec, p->m_rec);
    case 'cInt': return cmp3(m_int, p->m_int);
    case 'cWis': return cmp3(m_wis, p->m_wis);
    case 'cLuc': return cmp3(m_luc, p->m_luc);
    case 'sped': return cmp3(m_speed, p->m_speed);
    case 'exp ': return cmp3(m_exp, p->m_exp);
    case 'buy ': return cmp3(m_buy, p->m_buy);
    case 'sell': return cmp3(m_sell, p->m_sell);
    case 'grde': return cmp3(m_grade, p->m_grade);
    case 'drop': return cmp3(m_drop, p->m_drop);
    case 'srvr': return cmp3(m_server, p->m_server);
    case 'cnt ': return cmp3(m_count, p->m_count);
    case 'dura': return cmp3(m_duration, p->m_duration);
    case 'edur': return cmp3(m_extDuration, p->m_extDuration);
    case 'sopt': return cmp3(m_secOption, p->m_secOption);
    case 'orat': return cmp3(m_optionRate, p->m_optionRate);
    case 'bmth': return cmp3(m_buyMethod, p->m_buyMethod);
    case 'mxlv': return cmp3(m_maxLevel, p->m_maxLevel);
    case 'wprt': return cmp3(m_weaponPart, p->m_weaponPart);
    case 'itsk': return cmp3(m_itemSkill, p->m_itemSkill);
    case 'arg3': return cmp3(m_arg3, p->m_arg3);
    case 'arg4': return cmp3(m_arg4, p->m_arg4);
    case 'argA': return cmp3(m_arg10, p->m_arg10);
    case 'argC': return cmp3(m_arg12, p->m_arg12);
    case 'dyty': return cmp3(m_dyeingType, p->m_dyeingType);
    case 'ucty': return cmp3(m_useConType, p->m_useConType);
    case 'ucvr': return cmp3(m_useConVar, p->m_useConVar);
    case 'mnty': return cmp3(m_moneyType, p->m_moneyType);
    case 'itup': return cmp3(m_itemUpgrade, p->m_itemUpgrade);
    case 'gnct': return cmp3(m_geneCount, p->m_geneCount);
    case 'spex': return cmp3(m_spellBookExp, p->m_spellBookExp);
    case 'spdu': return cmp3(m_spellBookDurability, p->m_spellBookDurability);
    default:
        return 0;
    }
}

int Item::compare3(const Node* other, DWORD key1, DWORD key2, DWORD key3) const
{
    int result = compare(other, key1);
    if (!result && (!key2 || (result = compare(other, key2)) == 0))
    {
        if (key3)
            return compare(other, key3);
    }
    return result;
}

bool Item::assign(const Item* src)
{
    if (src->m_type == m_type && (!m_valid || src->m_typeId == m_typeId))
    {
        if (&src->m_name != &m_name)
        {
            m_name.clear();
            m_name.append(src->m_name.m_buffer, src->m_name.m_length);
        }
        if (&src->m_desc != &m_desc)
        {
            m_desc.clear();
            m_desc.append(src->m_desc.m_buffer, src->m_desc.m_length);
        }
        m_model = src->m_model;
        m_icon = src->m_icon;
        m_level = src->m_level;
        m_country = src->m_country;
        for (int i = 0; i < 6; ++i)
            m_class[i] = src->m_class[i];
        m_grow = src->m_grow;
        m_type2 = src->m_type2;
        m_type3 = src->m_type3;
        m_reqStr = src->m_reqStr;
        m_reqDex = src->m_reqDex;
        m_reqRec = src->m_reqRec;
        m_reqInt = src->m_reqInt;
        m_reqWis = src->m_reqWis;
        m_reqLuc = src->m_reqLuc;
        m_reqVg = src->m_reqVg;
        m_reqOg = src->m_reqOg;
        m_reqIg = src->m_reqIg;
        m_range = src->m_range;
        m_attackTime = src->m_attackTime;
        m_attrib = src->m_attrib;
        m_special = src->m_special;
        m_slot = src->m_slot;
        m_quality = src->m_quality;
        m_attack = src->m_attack;
        m_attackAdd = src->m_attackAdd;
        m_defense = src->m_defense;
        m_resist = src->m_resist;
        m_hp = src->m_hp;
        m_sp = src->m_sp;
        m_mp = src->m_mp;
        m_str = src->m_str;
        m_dex = src->m_dex;
        m_rec = src->m_rec;
        m_int = src->m_int;
        m_wis = src->m_wis;
        m_luc = src->m_luc;
        m_speed = src->m_speed;
        m_exp = src->m_exp;
        m_buy = src->m_buy;
        m_sell = src->m_sell;
        m_grade = src->m_grade;
        m_drop = src->m_drop;
        m_server = src->m_server;
        m_count = src->m_count;
        m_duration = src->m_duration;
        m_extDuration = src->m_extDuration;
        m_secOption = src->m_secOption;
        m_optionRate = src->m_optionRate;
        m_buyMethod = src->m_buyMethod;
        m_maxLevel = src->m_maxLevel;
        m_weaponPart = src->m_weaponPart;
        m_dyeingType = src->m_dyeingType;
        m_arg3 = src->m_arg3;
        m_arg4 = src->m_arg4;
        m_useConType = src->m_useConType;
        m_useConVar = src->m_useConVar;
        m_moneyType = src->m_moneyType;
        m_itemSkill = src->m_itemSkill;
        m_itemUpgrade = src->m_itemUpgrade;
        m_arg10 = src->m_arg10;
        m_geneCount = src->m_geneCount;
        m_arg12 = src->m_arg12;
        m_spellBookExp = src->m_spellBookExp;
        m_spellBookDurability = src->m_spellBookDurability;
        m_valid = src->m_valid;
        m_details.clear();
        return true;
    }
    MessageBeep(MB_ICONEXCLAMATION);
    return false;
}

bool Item::diff(const Item* other, String& out) const
{
    out.clear();
    for (int k = 4; g_itemColumnTags[k]; ++k)
    {
        DWORD tag = g_itemColumnTags[k];
        if (compare(other, tag))
        {
            if (out.m_length)
                out << L'\t';
            (out << g_itemColumnNames[k] << L':' << (long)getInt(tag)) << L'/' << (long)other->getInt(tag);
        }
    }
    return out.m_length != 0;
}

void Item::read(iStream& in, DWORD fmt)
{
    FlntRowRead<Item> flntRow(this, in, fmt);
    switch (fmt)
    {
    case 'ltxt':
        m_name.read(in, 4, (UINT)-1);
        m_desc.read(in, 4, (UINT)-1);
        {
            String text = m_desc.unescape();
            if (&text != &m_desc)
            {
                m_desc.clear();
                m_desc.append(text.m_buffer, text.m_length);
            }
        }
        break;
    case 'file':
    {
        m_name.read(in, 4, in.m_codePage);
        m_desc.read(in, 4, in.m_codePage);
        m_desc = m_desc.unescape();
        m_type = in.readByte();
        m_typeId = in.readByte();
        m_model = in.readByte();
        m_icon = in.readByte();
        m_level = in.readShort();
        m_country = in.readByte();
        for (int i = 0; i < 6; ++i)
            m_class[i] = in.readByte();
        m_grow = in.readByte();
        m_type2 = in.readByte();
        m_type3 = in.readByte();
        m_reqStr = in.readShort();
        m_reqDex = in.readShort();
        m_reqRec = in.readShort();
        m_reqInt = in.readShort();
        m_reqWis = in.readShort();
        m_reqLuc = in.readShort();
        m_reqVg = in.readShort();
        if (m_version >= 0x620)
            m_unk72 = in.readShort();
        m_reqOg = in.readByte();
        m_reqIg = in.readByte();
        if (m_version >= 0x600)
            m_range = in.readShort();
        else
        {
            BYTE range = (BYTE)in.readByte();
            m_range = range;
            if (m_type == 44 || m_type == 100)
            {
                if (range && range < 45)
                    m_range = range + 256;
                else if (!range && m_type == 100 && (m_typeId == 95 || m_typeId == 96 || m_typeId == 97))
                    m_range = 256;
            }
        }
        m_attackTime = in.readByte();
        m_attrib = in.readByte();
        m_special = in.readByte();
        m_slot = in.readByte();
        m_quality = in.readShort();
        m_attack = in.readShort();
        m_attackAdd = in.readShort();
        m_defense = in.readShort();
        m_resist = in.readShort();
        m_hp = in.readShort();
        m_sp = in.readShort();
        m_mp = in.readShort();
        m_str = in.readShort();
        m_dex = in.readShort();
        m_rec = in.readShort();
        m_int = in.readShort();
        m_wis = in.readShort();
        m_luc = in.readShort();
        m_speed = in.readByte();
        m_exp = in.readByte();
        m_buy = in.readInt();
        m_sell = in.readInt();
        m_grade = in.readShort();
        m_drop = in.readShort();
        m_server = in.readByte();
        m_count = in.readByte();
        if (m_version >= 0x600)
        {
            m_duration = in.readInt();
            m_extDuration = in.readByte();
            m_secOption = in.readByte();
            m_optionRate = in.readByte();
            m_buyMethod = in.readByte();
            m_maxLevel = in.readByte();
            m_weaponPart = in.readByte();
            m_dyeingType = in.readByte();
            m_arg3 = in.readInt();
            m_arg4 = in.readInt();
            m_useConType = in.readInt();
            m_useConVar = in.readInt();
            m_moneyType = in.readInt();
            m_itemSkill = in.readInt();
            m_itemUpgrade = in.readInt();
            m_arg10 = in.readInt();
            m_geneCount = in.readByte();
            m_arg12 = in.readByte();
            m_spellBookExp = in.readByte();
            m_spellBookDurability = in.readInt();
        }
        break;
    }
    case 'flnt':
        m_type = (BYTE)in.readInt64();
        m_typeId = (BYTE)in.readInt64();
        m_model = (BYTE)in.readInt64();
        m_icon = (BYTE)in.readInt64();
        m_level = (short)in.readInt64();
        m_country = (BYTE)in.readInt64();
        for (int i = 0; i < 6; ++i)
            m_class[i] = (BYTE)in.readInt64();
        m_grow = (BYTE)in.readInt64();
        m_reqStr = (WORD)in.readInt64();
        m_reqDex = (WORD)in.readInt64();
        m_reqRec = (WORD)in.readInt64();
        m_reqInt = (WORD)in.readInt64();
        m_reqWis = (WORD)in.readInt64();
        m_reqLuc = (WORD)in.readInt64();
        m_reqVg = (short)in.readInt64();
        m_reqOg = (BYTE)in.readInt64();
        m_reqIg = (BYTE)in.readInt64();
        m_range = (short)in.readInt64();
        m_attackTime = (BYTE)in.readInt64();
        m_attrib = (BYTE)in.readInt64();
        m_special = (BYTE)in.readInt64();
        m_slot = (BYTE)in.readInt64();
        m_quality = (WORD)in.readInt64();
        m_attack = (WORD)in.readInt64();
        m_attackAdd = (WORD)in.readInt64();
        m_defense = (WORD)in.readInt64();
        m_resist = (WORD)in.readInt64();
        m_hp = (WORD)in.readInt64();
        m_sp = (WORD)in.readInt64();
        m_mp = (WORD)in.readInt64();
        m_str = (WORD)in.readInt64();
        m_dex = (WORD)in.readInt64();
        m_rec = (WORD)in.readInt64();
        m_int = (WORD)in.readInt64();
        m_wis = (WORD)in.readInt64();
        m_luc = (WORD)in.readInt64();
        m_speed = (BYTE)in.readInt64();
        m_exp = (BYTE)in.readInt64();
        m_buy = (int)in.readInt64();
        m_sell = (int)in.readInt64();
        m_grade = (short)in.readInt64();
        m_drop = (short)in.readInt64();
        m_server = (BYTE)in.readInt64();
        m_count = (BYTE)in.readInt64();
        m_duration = (int)in.readInt64();
        m_extDuration = (BYTE)in.readInt64();
        m_secOption = (BYTE)in.readInt64();
        m_optionRate = (BYTE)in.readInt64();
        m_buyMethod = (BYTE)in.readInt64();
        m_maxLevel = (BYTE)in.readInt64();
        m_weaponPart = (BYTE)in.readInt64();
        m_dyeingType = (BYTE)in.readInt64();
        m_arg3 = (int)in.readInt64();
        m_arg4 = (int)in.readInt64();
        m_useConType = (int)in.readInt64();
        m_useConVar = (int)in.readInt64();
        m_moneyType = (int)in.readInt64();
        m_itemSkill = (int)in.readInt64();
        m_itemUpgrade = (int)in.readInt64();
        m_arg10 = (int)in.readInt64();
        m_geneCount = (BYTE)in.readInt64();
        m_arg12 = (BYTE)in.readInt64();
        m_spellBookExp = (BYTE)in.readInt64();
        m_spellBookDurability = (int)in.readInt64();
        return;
    default:
        return;
    }
    m_valid = !(m_name.m_length && m_name.m_buffer[0] == L'?');
}

void Item::write(oStream& out, DWORD fmt) const
{
    FlntRowWrite flntRow(this, out, fmt);
    switch (fmt)
    {
    case 'ltxt':
    {
        out << (__int64)m_type;
        out << (__int64)m_typeId;
        m_name.write(out, 4, (UINT)-1);
        m_desc.escape().write(out, 4, (UINT)-1);
        break;
    }
    case 'file':
    {
        String desc = m_desc.escape();
        m_name.write(out, 4, out.m_codePage);
        desc.write(out, 4, out.m_codePage);
        out << (char)m_type << (char)m_typeId << (char)m_model << (char)m_icon << (unsigned short)m_level << (char)m_country;
        out << (char)m_class[0] << (char)m_class[1] << (char)m_class[2] << (char)m_class[3] << (char)m_class[4] << (char)m_class[5];
        out << (char)m_grow << (char)m_type2 << (char)m_type3;
        out << (short)m_reqStr << (short)m_reqDex << (short)m_reqRec << (short)m_reqInt << (short)m_reqWis << (short)m_reqLuc
            << (unsigned short)m_reqVg;
        if (m_version >= 0x620)
            out << (unsigned short)m_unk72;
        out << (char)m_reqOg << (char)m_reqIg;
        if (m_version >= 0x600)
            out << (unsigned short)m_range;
        else
            out << (char)(BYTE)m_range;
        out << (char)m_attackTime << (char)m_attrib << (char)m_special << (char)m_slot
            << (short)m_quality << (short)m_attack << (short)m_attackAdd << (short)m_defense << (short)m_resist
            << (short)m_hp << (short)m_sp << (short)m_mp << (short)m_str << (short)m_dex << (short)m_rec
            << (short)m_int << (short)m_wis << (short)m_luc
            << (char)m_speed << (char)m_exp << (unsigned long)m_buy << (unsigned long)m_sell
            << (unsigned short)m_grade << (unsigned short)m_drop << (char)m_server << (char)m_count;
        if (m_version >= 0x600)
        {
            out << (unsigned long)m_duration << (char)m_extDuration << (char)m_secOption << (char)m_optionRate
                << (char)m_buyMethod << (char)m_maxLevel << (char)m_weaponPart << (char)m_dyeingType
                << (unsigned long)m_arg3 << (unsigned long)m_arg4 << (unsigned long)m_useConType
                << (unsigned long)m_useConVar << (unsigned long)m_moneyType << (unsigned long)m_itemSkill
                << (unsigned long)m_itemUpgrade << (unsigned long)m_arg10
                << (char)m_geneCount << (char)m_arg12 << (char)m_spellBookExp << (unsigned long)m_spellBookDurability;
        }
        break;
    }
    case 'flnt':
    {
        out << (__int64)m_type;
        out << (__int64)m_typeId;
        out << (__int64)m_model;
        out << (__int64)m_icon;
        out << (__int64)m_level;
        out << (__int64)m_country;
        for (int i = 0; i < 6; ++i)
            out << (__int64)m_class[i];
        out << (__int64)m_grow;
        out << (__int64)m_reqStr;
        out << (__int64)m_reqDex;
        out << (__int64)m_reqRec;
        out << (__int64)m_reqInt;
        out << (__int64)m_reqWis;
        out << (__int64)m_reqLuc;
        out << (__int64)m_reqVg;
        out << (__int64)m_reqOg;
        out << (__int64)m_reqIg;
        out << (__int64)m_range;
        out << (__int64)m_attackTime;
        out << (__int64)m_attrib;
        out << (__int64)m_special;
        out << (__int64)m_slot;
        out << (__int64)m_quality;
        out << (__int64)m_attack;
        out << (__int64)m_attackAdd;
        out << (__int64)m_defense;
        out << (__int64)m_resist;
        out << (__int64)m_hp;
        out << (__int64)m_sp;
        out << (__int64)m_mp;
        out << (__int64)m_str;
        out << (__int64)m_dex;
        out << (__int64)m_rec;
        out << (__int64)m_int;
        out << (__int64)m_wis;
        out << (__int64)m_luc;
        out << (__int64)m_speed;
        out << (__int64)m_exp;
        out << (__int64)m_buy;
        out << (__int64)m_sell;
        out << (__int64)m_grade;
        out << (__int64)m_drop;
        out << (__int64)m_server;
        out << (__int64)m_count;
        out << (__int64)m_duration;
        out << (__int64)m_extDuration;
        out << (__int64)m_secOption;
        out << (__int64)m_optionRate;
        out << (__int64)m_buyMethod;
        out << (__int64)m_maxLevel;
        out << (__int64)m_weaponPart;
        out << (__int64)m_dyeingType;
        out << (__int64)m_arg3;
        out << (__int64)m_arg4;
        out << (__int64)m_useConType;
        out << (__int64)m_useConVar;
        out << (__int64)m_moneyType;
        out << (__int64)m_itemSkill;
        out << (__int64)m_itemUpgrade;
        out << (__int64)m_arg10;
        out << (__int64)m_geneCount;
        out << (__int64)m_arg12;
        out << (__int64)m_spellBookExp;
        out << (__int64)m_spellBookDurability;
        break;
    }
    }
}

bool Item::toText(String& out, DWORD fmt) const
{
    if (fmt == 'sql ')
    {
        String values;
        values.m_flags = SF_USHORT_SIGNED;
        {
            String name = String::sqlQuote(m_name.m_buffer);
            String& v = values << (long)(m_typeId + 1000 * m_type) << L',';
            v.append(name.m_buffer, name.m_length) << L',';
            v << (unsigned char)m_type << L',' << (unsigned char)m_typeId << L',' << (short)m_level << L','
              << (unsigned char)m_country << L',';
            for (int i = 0; i < 6; ++i)
                v << (unsigned char)m_class[i] << L',';
            v << (unsigned char)m_grow << L','
              << (unsigned short)m_reqStr << L',' << (unsigned short)m_reqDex << L',' << (unsigned short)m_reqRec << L','
              << (unsigned short)m_reqInt << L',' << (unsigned short)m_reqWis << L',' << (unsigned short)m_reqLuc << L','
              << (short)m_reqVg << L',' << (unsigned char)m_reqOg << L',' << (unsigned char)m_reqIg << L','
              << (short)m_range << L','
              << (unsigned char)m_attackTime << L',' << (unsigned char)m_attrib << L',' << (unsigned char)m_special << L','
              << (unsigned char)m_slot << L','
              << (unsigned short)m_quality << L',' << (unsigned short)m_attack << L',' << (unsigned short)m_attackAdd << L','
              << (unsigned short)m_defense << L',' << (unsigned short)m_resist << L','
              << (unsigned short)m_hp << L',' << (unsigned short)m_sp << L',' << (unsigned short)m_mp << L','
              << (unsigned short)m_str << L',' << (unsigned short)m_dex << L',' << (unsigned short)m_rec << L','
              << (unsigned short)m_int << L',' << (unsigned short)m_wis << L',' << (unsigned short)m_luc << L','
              << (unsigned char)m_speed << L',' << (unsigned char)m_exp << L','
              << (long)m_buy << L',' << (long)m_sell << L',' << (short)m_grade << L',' << (short)m_drop << L','
              << (unsigned char)m_server << L',' << (unsigned char)m_count;
        }
        out << L"INSERT INTO [PS_GameDefs].[dbo].[Items] ";
        out << L"([ItemID],[ItemName],[Type],[TypeID],[Reqlevel],[Country],[Attackfighter],[Defensefighter],[Patrolrogue],[Shootrog"
               L"ue],[Attackmage],[Defensemage],[Grow],[ReqStr],[ReqDex],[ReqRec],[ReqInt],[ReqWis],[Reqluc],[ReqVg],[ReqOg],[ReqI"
               L"g],[Range],[AttackTime],[Attrib],[Special],[Slot],[Quality],[Effect1],[Effect2],[Effect3],[Effect4],[ConstHP],[Co"
               L"nstSP],[ConstMP],[ConstStr],[ConstDex],[ConstRec],[ConstInt],[ConstWis],[ConstLuc],[Speed],[Exp],[Buy],[Sell],[Gr"
               L"ade],[Drop],[Server],[Count]) ";
        String& s = out << L"values (";
        s.append(values.m_buffer, values.m_length) << L");" << L"\r\n";
        return true;
    }
    if (!m_valid && fmt != 'csve')
        return false;
    String desc = m_desc.escape();
    String& a = out << (unsigned char)m_type << L';' << (unsigned char)m_typeId << L';' << L'"';
    a.append(m_name.m_buffer, m_name.m_length) << L'"' << L';' << L'"';
    a.append(desc.m_buffer, desc.m_length) << L'"' << L';';
    a << (unsigned char)m_model << L';' << (unsigned char)m_icon << L';' << (short)m_level << L';'
      << (unsigned char)m_country << L';';
    for (int i = 0; i < 6; ++i)
        a << (unsigned char)m_class[i] << L';';
    a << (unsigned char)m_grow << L';' << (unsigned char)m_type2 << L';' << (unsigned char)m_type3 << L';'
      << (unsigned short)m_reqStr << L';' << (unsigned short)m_reqDex << L';' << (unsigned short)m_reqRec << L';'
      << (unsigned short)m_reqInt << L';' << (unsigned short)m_reqWis << L';' << (unsigned short)m_reqLuc << L';'
      << (short)m_reqVg << L';' << (unsigned char)m_reqOg << L';' << (unsigned char)m_reqIg << L';'
      << (short)m_range << L';'
      << (unsigned char)m_attackTime << L';' << (unsigned char)m_attrib << L';' << (unsigned char)m_special << L';'
      << (unsigned char)m_slot << L';'
      << (unsigned short)m_quality << L';' << (unsigned short)m_attack << L';' << (unsigned short)m_attackAdd << L';'
      << (unsigned short)m_defense << L';' << (unsigned short)m_resist << L';'
      << (unsigned short)m_hp << ';' << (unsigned short)m_sp << ';' << (unsigned short)m_mp << ';'
      << (unsigned short)m_str << ';' << (unsigned short)m_dex << ';' << (unsigned short)m_rec << ';'
      << (unsigned short)m_int << ';' << (unsigned short)m_wis << ';' << (unsigned short)m_luc << ';'
      << (unsigned char)m_speed << ';' << (unsigned char)m_exp << ';'
      << (long)m_buy << ';' << (long)m_sell << ';' << (short)m_grade << ';' << (short)m_drop << ';'
      << (unsigned char)m_server << ';' << (unsigned char)m_count << L"\r\n";
    return true;
}

int Item::classFromFlags(short flags)
{
    if ((flags & 1) != 0 || (flags & 2) != 0)
        return 0;
    if ((flags & 4) != 0 || (flags & 8) != 0)
        return 2;
    if ((flags & 0x20) != 0)
        return 3;
    if ((flags & 0x10) != 0)
        return 1;
    if ((flags & 0x100) != 0 || (flags & 0x200) != 0)
        return 4;
    if ((flags & 0x400) != 0)
        return 6;
    if ((flags & 0x800) != 0)
        return 5;
    if ((flags & 0x2000) != 0)
        return 7;
    return (flags & 0x1000) != 0 ? 7 : -2;
}

char Item::baseType(char type)
{
    switch (type)
    {
    case 1: case 45: return 1;
    case 2: case 46: return 2;
    case 3: case 47: return 3;
    case 4: case 48: return 4;
    case 5: case 49: case 50: return 5;
    case 6: case 51: case 52: return 6;
    case 7: case 53: case 54: return 7;
    case 8: case 55: case 56: return 8;
    case 9: case 57: return 9;
    case 10: case 58: return 10;
    case 11: case 59: return 11;
    case 12: case 60: case 61: return 12;
    case 13: case 62: case 63: return 13;
    case 14: case 64: return 14;
    case 15: case 65: return 15;
    case 19: case 69: case 75: return 19;
    case 34: case 84: case 90: return 34;
    default: return type;
    }
}

bool Item::isGearType(char type, bool second)
{
    if (second)
    {
        if (((unsigned char)(type - 16) <= 5u && type != 19)
            || ((unsigned char)(type - 66) <= 5u && type != 69)
            || ((unsigned char)(type - 72) <= 5u && type != 75))
            return true;
    }
    else
    {
        if (((unsigned char)(type - 31) <= 5u && type != 34) || ((unsigned char)(type - 81) <= 5u && type != 84))
            return true;
        if ((unsigned char)(type - 87) <= 5u && type != 90)
            return true;
    }
    return false;
}

bool Item::matchesType(const Item* item, char type)
{
    BYTE t = item->m_type;
    switch (type)
    {
    case 16: return t == 16 || t == 66 || t == 72;
    case 17: return t == 17 || t == 67 || t == 73;
    case 18: return t == 18 || t == 68 || t == 74;
    case 20: return t == 20 || t == 70 || t == 76;
    case 21: return t == 21 || t == 71 || t == 77;
    case 31: return t == 31 || t == 81 || t == 87;
    case 32: return t == 32 || t == 82 || t == 88;
    case 33: return t == 33 || t == 83 || t == 89;
    case 35: return t == 35 || t == 85 || t == 91;
    case 36: return t == 36 || t == 86 || t == 92;
    default: return false;
    }
}

int Item::equipSlot(int type)
{
    switch ((unsigned)type)
    {
    case 0x10: case 0x1F: case 0x42: case 0x48: case 0x51: case 0x57: return 4;
    case 0x11: case 0x20: case 0x43: case 0x49: case 0x52: case 0x58: return 6;
    case 0x12: case 0x21: case 0x44: case 0x4A: case 0x53: case 0x59: return 5;
    case 0x14: case 0x23: case 0x46: case 0x4C: case 0x55: case 0x5B: return 3;
    case 0x15: case 0x24: case 0x47: case 0x4D: case 0x56: case 0x5C: return 1;
    default: return -1;
    }
}

}

void ptrArray_detachAll(ptrArray<Data::Item*>& list)
{
    for (int i = 0; i < list.m_count; ++i)
        list.m_items[i] = 0;
    list.m_count = 0;
}
