#pragma once
#include "p02_common.h"
#include "Strings.h"

extern const wchar_t* const g_monsterAnimationNames[18];
extern wchar_t g_monsterEffectsText[256];

class MonsterModel : public Node
{
public:
    struct Effect
    {
        int m_id;
        int m_value;
    };

    class Effects : public ptrArray<Effect*>
    {
    public:
        Effects() : ptrArray<Effect*>(256) {}
        virtual ~Effects() {}

        void assign(const Effects& src);
        void read(iStream& in, DWORD fmt);
        bool toText(String& out, DWORD fmt) const;
    };
    static_assert(sizeof(Effects) == 0x18, "MonsterModel::Effects size");

    MonsterModel(DWORD format, int id, int unused = 0);
    virtual ~MonsterModel();

    void read(iStream& in, DWORD fmt) override;
    void write(oStream& out, DWORD fmt) const override;
    bool toText(String& out, DWORD fmt) const override;
    bool copyFrom(const Streamable* src) override;
    const wchar_t* getString(DWORD tag) const override;
    int  getInt(DWORD tag) const override;
    bool setInt(DWORD tag, int value) override;
    bool setString(DWORD tag, const wchar_t* value) override;
    void reset() override;
    int  compare(const Node* other, DWORD tag) const override;

    void assign(const MonsterModel& src);
    const wchar_t* animationName(long id) const;

    DWORD   m_format;
    int     m_id;
    String  m_name;
    BYTE    m_unk1;
    Strings m_animations;
    Strings m_vertices;
    Strings m_textures;
    float   m_height;
    Effects m_effects;
};
static_assert(sizeof(MonsterModel) == 0xE0, "MonsterModel size");
