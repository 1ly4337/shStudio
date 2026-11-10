#pragma once
#include <windows.h>
#include "ptrArray.h"
#include "Serializable.h"
#include "Monster.h"

class Monster;
class iStream;
class oStream;
class String;

class Monsters : public ptrArray<Monster*>, public Serializable
{
public:
    Monsters();
    virtual ~Monsters();

    bool isEmpty() const override { return m_count == 0; }
    void clear() override;
    const wchar_t* getFilePrefix() const override;
    void load(iStream& in, int arg2, int arg3) override;
    HRESULT setCodePage(int format) override;

    void read(iStream& in, DWORD fmt) override;
    void write(oStream& out, DWORD fmt) const override;
    bool toText(String& out, DWORD fmt) const override;

    virtual void setExtended(bool extended);

    Monster* findById(int id);
    static void readHeader(iStream& s);
    static void writeHeader(oStream& s);

    int  m_format;
    bool m_extended;
};
static_assert(sizeof(Monsters) == 0x3C, "Monsters size");
