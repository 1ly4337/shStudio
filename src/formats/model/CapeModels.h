#pragma once
#include "Serializable.h"
#include "Strings.h"
#include "CapesData.h"

class String;
class iStream;
class oStream;

class CapeModels : public Serializable
{
public:
    CapeModels();
    virtual ~CapeModels();

    virtual bool isEmpty() const;
    virtual void clear();
    virtual void read(iStream& in, DWORD fmt);
    virtual void write(oStream& out, DWORD fmt) const;
    virtual bool toText(String& out, DWORD fmt) const;

public:
    Strings   m_capeFiles;
    Strings   m_shoulderFiles;
    CapesData m_capes;
};
static_assert(sizeof(CapeModels) == 0x8C, "CapeModels size");
