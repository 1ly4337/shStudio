#pragma once
#include "p02_common.h"

class Monster : public Node
{
public:
    struct Attack
    {
        BYTE  ani;
        short type;
        int   time;
        BYTE  range;
        WORD  damage;
        WORD  add;
        BYTE  attrib;
        BYTE  special;
        BYTE  mode;

        void read(iStream& in, DWORD tag);
        void write(oStream& out, DWORD tag) const;
    };
    static_assert(sizeof(Attack) == 0x14, "Monster::Attack size");

    Monster(int id, int version, int unused = 0);
    virtual ~Monster();

    void read(iStream& in, DWORD fmt) override;
    void write(oStream& out, DWORD fmt) const override;
    bool toText(String& out, DWORD fmt) const override;
    int  getVersion() const override;
    bool copyFrom(const Streamable* src) override;
    const wchar_t* getString(DWORD tag) const override;
    int  getInt(DWORD tag) const override;
    bool setInt(DWORD tag, int value) override;
    bool setString(DWORD tag, const wchar_t* text) override;
    void reset() override;
    int  compare(const Node* node, DWORD tag) const override;
    virtual int  nodeType() const;
    virtual void assignFrom(Monster* src);

    int    m_version;
    bool   m_extended;
    int    m_id;
    String m_name;
    short  m_model;
    short  m_3A;
    short  m_level;
    WORD   m_exp;
    BYTE   m_ai;
    WORD   m_gold1;
    WORD   m_gold2;
    WORD   m_dropGroup[9];
    int    m_dropRate[9];
    short  m_quest;
    int    m_hp;
    WORD   m_sp;
    WORD   m_mp;
    WORD   m_dex;
    WORD   m_wis;
    WORD   m_luc;
    BYTE   m_day;
    BYTE   m_size;
    BYTE   m_attrib;
    WORD   m_defense;
    WORD   m_magic;
    BYTE   m_resist[15];
    BYTE   m_rk[6];
    int    m_normalTime;
    BYTE   m_normalStep;
    int    m_chaseTime;
    BYTE   m_chaseStep;
    BYTE   m_chaseRange;
    Attack m_attack[3];
    short  m_colorType;
    BYTE   m_colorHue;
    BYTE   m_colorSat;
    BYTE   m_colorLight;
};
static_assert(sizeof(Monster) == 0x108, "Monster size");

String sub_43CCF0(const wchar_t* prefix, int n);
String& sub_43D120(String& s, const void* attack);
