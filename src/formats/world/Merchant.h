#pragma once
#include "StandardNPC.h"
#include "primitiveArray.h"

class Merchant : public StandardNPC {
public:
    Merchant();
    virtual ~Merchant();

    virtual const wchar_t* getString(DWORD tag) const;
    virtual int  getInt(DWORD tag) const;
    virtual bool setInt(DWORD tag, int value);
    virtual void reset();
    virtual int  compare(const Node* other, DWORD tag) const;
    virtual void assign(const StandardNPC* src);
    virtual void readMain(iStream& s, int tag);
    virtual void writeMain(oStream& s, int tag) const;
    virtual void read(iStream& s, DWORD tag);
    virtual void write(oStream& s, DWORD tag) const;

    static int profIndex(unsigned char prof);

public:
    uint8_t              m_prof;
    uint8_t              _padCD[3];
    primitiveArray<long> m_items;
    uint8_t              m_lastEmpty;
};
static_assert(sizeof(Merchant) == 0xEC, "Merchant size");

extern const wchar_t* g_merchantTypeNames[18];
extern wchar_t g_merchantItemsBuf[0x200];
