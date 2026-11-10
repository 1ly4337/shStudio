#pragma once
#include <windows.h>
#include "ptrArray.h"
#include "Serializable.h"
#include "MobsSkill.h"

class iStream;
class oStream;
class String;

class MobsSkills : public ptrArray<MobsSkill*>, public Serializable
{
public:
    MobsSkills();
    virtual ~MobsSkills();

    bool isEmpty() const override;

    void clear() override;
    const wchar_t* getFilePrefix() const override;
    void load(iStream& in, int arg2, int arg3) override;
    int serialV5() const override;
    HRESULT setCodePage(int codePage) override;

    void read(iStream& in, DWORD fmt) override;
    void write(oStream& out, DWORD fmt) const override;
    bool toText(String& out, DWORD fmt) const override;

public:
    int m_format;
};
static_assert(sizeof(MobsSkills) == 0x38, "MobsSkills size");
