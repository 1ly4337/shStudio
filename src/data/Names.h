#pragma once
#include "ptrArray.h"
#include "Streamable.h"
#include "Name.h"

class Name;
class iStream;
class oStream;
class String;

class Names : public ptrArray<Name*>, public Streamable
{
public:
    Names() : m_owner(0) {}
    virtual ~Names() {}

    void read(iStream& in, DWORD fmt) override;
    void write(oStream& out, DWORD fmt) const override;
    bool toText(String& out, DWORD fmt) const override;

    virtual const wchar_t* nameOf(int id);
    virtual int namesV14(int arg) { return 0; }

    int m_owner;
};
static_assert(sizeof(Names) == 0x20, "Names size");
