#pragma once
#include <windows.h>
#include "ida_compat.h"
#include "Node.h"
#include "String.hpp"

#pragma warning(disable : 4066 4250)

class ToggleNode : public Node
{
public:
    explicit ToggleNode(const wchar_t* caption);
    virtual ~ToggleNode() {}

    virtual const wchar_t* getString(DWORD tag) const override;
    virtual int getInt(DWORD tag) const override { return tag == '1col'; }
    virtual void reset() override {}
    virtual int compare(const Node* other, DWORD tag) const override { return 0; }

    virtual void read(iStream& in, DWORD fmt) override {}
    virtual void write(oStream& out, DWORD fmt) const override {}

public:
    String m_caption;
};
