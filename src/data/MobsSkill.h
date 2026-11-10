#pragma once
#include "Node.h"
#include "String.hpp"
#include "SkillData.h"

class iStream;
class oStream;
class Streamable;

class MobsSkill : public Node
{
public:
    MobsSkill(int version, short skillId, int unused);
    virtual ~MobsSkill();

    int            getId() const override;
    const wchar_t* getString(DWORD tag) const override;
    int            getInt(DWORD tag) const override;
    bool           setInt(DWORD tag, int value) override;
    bool           setString(DWORD tag, const wchar_t* value) override;
    void           reset() override;
    int            compare(const Node* other, DWORD tag) const override;

    void read(iStream& in, DWORD fmt) override;
    void write(oStream& out, DWORD fmt) const override;
    bool toText(String& out, DWORD fmt) const override;
    bool copyFrom(const Streamable* src) override;

    MobsSkill& operator=(const MobsSkill& other);

    String    m_name;
    String    m_desc;
    SkillData m_skill;
};

static_assert(sizeof(MobsSkill) == 0xF8, "MobsSkill size");
