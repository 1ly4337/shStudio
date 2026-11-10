#pragma once
#include "p02_common.h"

class RecordSet;

namespace Data {

class Item : public Node
{
public:
    Item(int index = 0, int version = 0, int unused = 0);
    virtual ~Item();

    void read(iStream& in, DWORD fmt) override;
    void write(oStream& out, DWORD fmt) const override;
    bool toText(String& out, DWORD fmt) const override;
    int  getVersion() const override;
    bool copyFrom(const Streamable* src) override;
    int  getId() const override;
    const wchar_t* getString(DWORD tag) const override;
    int  getInt(DWORD tag) const override;
    bool setInt(DWORD tag, int value) override;
    bool setString(DWORD tag, const wchar_t* value) override;
    void reset() override;
    int  compare(const Node* other, DWORD tag) const override;
    virtual int  nodeType() const;
    virtual bool assign(const Item* src);
    virtual bool diff(const Item* other, String& out) const;
    virtual int  compare3(const Node* other, DWORD key1, DWORD key2, DWORD key3) const;

    static int  classFromFlags(short flags);
    static char baseType(char type);
    static bool isGearType(char type, bool second);
    static bool matchesType(const Item* item, char type);
    static int  equipSlot(int type);
    bool readSQLRow(RecordSet* rs, bool withName);

    int    m_index;
    int    m_version;
    String m_name;
    String m_desc;
    BYTE   m_type;
    BYTE   m_typeId;
    BYTE   m_model;
    BYTE   m_icon;
    short  m_level;
    BYTE   m_country;
    BYTE   m_class[6];
    BYTE   m_grow;
    BYTE   m_type2;
    BYTE   m_type3;
    WORD   m_reqStr;
    WORD   m_reqDex;
    WORD   m_reqRec;
    WORD   m_reqInt;
    WORD   m_reqWis;
    WORD   m_reqLuc;
    short  m_reqVg;
    WORD   m_unk72;
    BYTE   m_reqOg;
    BYTE   m_reqIg;
    short  m_range;
    BYTE   m_attackTime;
    BYTE   m_attrib;
    BYTE   m_special;
    BYTE   m_slot;
    WORD   m_quality;
    WORD   m_attack;
    WORD   m_attackAdd;
    WORD   m_defense;
    WORD   m_resist;
    WORD   m_hp;
    WORD   m_sp;
    WORD   m_mp;
    WORD   m_str;
    WORD   m_dex;
    WORD   m_rec;
    WORD   m_int;
    WORD   m_wis;
    WORD   m_luc;
    BYTE   m_speed;
    BYTE   m_exp;
    WORD   m_pad9A;
    int    m_buy;
    int    m_sell;
    short  m_grade;
    short  m_drop;
    BYTE   m_server;
    BYTE   m_count;
    WORD   m_padAA;
    int    m_duration;
    BYTE   m_extDuration;
    BYTE   m_secOption;
    BYTE   m_optionRate;
    BYTE   m_buyMethod;
    BYTE   m_maxLevel;
    BYTE   m_weaponPart;
    BYTE   m_dyeingType;
    BYTE   m_padB7;
    int    m_arg3;
    int    m_arg4;
    int    m_useConType;
    int    m_useConVar;
    int    m_moneyType;
    int    m_itemSkill;
    int    m_itemUpgrade;
    int    m_arg10;
    BYTE   m_geneCount;
    BYTE   m_arg12;
    BYTE   m_spellBookExp;
    BYTE   m_padDB;
    int    m_spellBookDurability;
    bool   m_valid;
    String m_details;
};

}

static_assert(sizeof(Data::Item) == 0x10C, "Data::Item size");

extern const wchar_t* g_itemColumnNames[54];
extern const wchar_t* g_itemColumnNamesEp6[21];
extern DWORD g_itemColumnTagsEp6[21];
extern DWORD g_itemColumnTags[54];
extern wchar_t g_itemTypeText[16];

void ptrArray_detachAll(ptrArray<Data::Item*>& list);
