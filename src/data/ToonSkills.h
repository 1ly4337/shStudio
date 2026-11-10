#pragma once
#include <windows.h>
#include "ptrArray.h"
#include "Serializable.h"
#include "ToonSkill.h"

class iStream;
class oStream;
class String;

class ToonSkills : public ptrArray<ToonSkill*>, public Serializable
{
public:
    ToonSkills();
    virtual ~ToonSkills();

    bool isEmpty() const override;

    void clear() override;
    const wchar_t* getFilePrefix() const override;
    void load(iStream& in, int arg2, int arg3) override;
    HRESULT setCodePage(int codePage) override;

    void read(iStream& in, DWORD fmt) override;
    void write(oStream& out, DWORD fmt) const override;
    bool toText(String& out, DWORD fmt) const override;

    ToonSkill* findById(int skillId) const;

public:
    int m_format;
};
static_assert(sizeof(ToonSkills) == 0x38, "ToonSkills size");
