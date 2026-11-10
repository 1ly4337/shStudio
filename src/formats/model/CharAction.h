#pragma once
#include "p02_common.h"

class CharAction : public Node
{
public:
    CharAction(int id, int unused = 0);
    virtual ~CharAction();

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

    void assign(const CharAction& src);

    int    m_id;
    String m_name;
    BYTE   m_mode;
    BYTE   m_byte1;
    BYTE   m_byte2;
    BYTE   m_byte3;
    int    m_unk1;
    float  m_floats[4];
};
static_assert(sizeof(CharAction) == 0x50, "CharAction size");
