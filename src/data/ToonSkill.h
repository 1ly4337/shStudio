#pragma once
#include "p04_common.h"
#include "Node.h"
#include "String.hpp"
#include "SkillData.h"

class iStream;
class oStream;

extern wchar_t g_toonSkillTypeText[256];

class ToonSkill : public Node
{
public:
    enum { MaxLevels = 15 };

    ToonSkill(int skillId, int format);
    virtual ~ToonSkill();

    virtual int getId() const override;
    virtual const wchar_t* getString(DWORD tag) const override;
    virtual int getInt(DWORD tag) const override;
    virtual bool setInt(DWORD tag, int value) override;
    virtual bool setString(DWORD tag, const wchar_t* value) override;
    virtual void reset() override;
    virtual int compare(const Node* other, DWORD tag) const override;
    virtual void setFormat(int format);
    virtual bool assign(const ToonSkill* other);

    virtual void read(iStream& in, DWORD fmt) override;
    virtual void write(oStream& out, DWORD fmt) const override;
    virtual bool toText(String& out, DWORD fmt) const override;
    virtual bool copyFrom(const Streamable* src) override;

    static void baseName(const String& src, String& dst);

public:
    int       m_format;
    int       m_levelCount;
    int       m_skillId;
    String    m_name;
    String    m_levelNames[MaxLevels];
    String    m_levelTexts[MaxLevels];
    SkillData m_levels[MaxLevels];
};
static_assert(sizeof(ToonSkill) == 0xD9C, "ToonSkill size");
