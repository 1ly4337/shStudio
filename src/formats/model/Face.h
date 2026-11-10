#pragma once
#include "Node.h"
#include "fPoint.h"

class String;
class iStream;
class oStream;

class Face : public Node
{
public:
    Face(short index);
    ~Face() override {}

    short          getImage(int tag, short def) const override;
    const wchar_t* getString(DWORD tag) const override;
    int            getInt(DWORD tag) const override;
    bool           setInt(DWORD tag, int value) override;
    void           reset() override;
    int            compare(const Node* other, DWORD tag) const override;
    virtual int    nodeTag();
    virtual int    compare(const Face* other, DWORD tag) const;

    void read(iStream& s, DWORD tag) override;
    void write(oStream& s, DWORD tag) const override;
    bool toText(String& out, DWORD fmt) const override;
    bool copyFrom(const Streamable* src) override;

public:
    unsigned short m_index;
    unsigned short m_v1;
    unsigned short m_v2;
    unsigned short m_v3;
    fPoint         m_normal;
    unsigned char  m_flags;
};
static_assert(sizeof(Face) == 0x30, "Face size");

extern wchar_t g_faceText[64];
