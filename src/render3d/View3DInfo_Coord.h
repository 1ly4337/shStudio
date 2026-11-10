#pragma once
#include <windows.h>
#include "View3DInfo.h"
#include "Node.h"

class View3DInfo::Coord : public Node
{
public:
    Coord(const wchar_t* name, float min, float max) : m_name(name), m_min(min), m_max(max) {}
    virtual ~Coord() {}

    const wchar_t* getString(DWORD tag) const override;
    int getInt(DWORD tag) const override;
    void reset() override {}
    int compare(const Node* other, DWORD tag) const override { return 0; }
    void read(iStream& in, DWORD fmt) override {}
    void write(oStream& out, DWORD fmt) const override {}

public:
    const wchar_t* m_name;
    float          m_min;
    float          m_max;
};
static_assert(sizeof(View3DInfo::Coord) == 0x20, "View3DInfo::Coord size");
