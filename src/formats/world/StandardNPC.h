#pragma once
#include <windows.h>
#include <cstdint>
#include "Node.h"
#include "String.hpp"
#include "vShort.h"
#include "iStream.h"
#include "oStream.h"

class StandardNPC : public Node {
public:
    explicit StandardNPC(int type);
    virtual ~StandardNPC();

    virtual const wchar_t* getString(DWORD tag) const;
    virtual int  getInt(DWORD tag) const;
    virtual bool setInt(DWORD tag, int value);
    virtual bool setString(DWORD tag, const wchar_t* text);
    virtual void reset();
    virtual int  compare(const Node* other, DWORD tag) const;

    virtual void trimStrings();
    virtual void clearLists();
    virtual void buildListStrings();
    virtual void assign(const StandardNPC* src);
    virtual void readMain(iStream& s, int tag);
    virtual void writeMain(oStream& s, int tag) const;
    virtual void readLists(iStream& s, int tag);
    virtual void writeLists(oStream& s, int tag) const;

    virtual void read(iStream& s, DWORD tag);
    virtual void write(oStream& s, DWORD tag) const;
    virtual bool toText(String& out, DWORD format) const;
    virtual bool copyFrom(const Streamable* src);

    static const wchar_t* typeName(int type);

public:
    int     m_type;
    int16_t m_id;
    int16_t _pad12;
    int     m_shape;
    int     m_unk1;
    int     m_unk2;
    int     m_country;
    String  m_name;
    String  m_welcome;
    vShort  m_questsIn;
    vShort  m_questsOut;
    String  m_questsInText;
    String  m_questsOutText;
};
static_assert(sizeof(StandardNPC) == 0xD4, "StandardNPC size");
