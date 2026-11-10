#pragma once
#include "Node.h"

class WorkspaceItemNode : public Node
{
public:
    WorkspaceItemNode() {}
    virtual ~WorkspaceItemNode() {}

    void read(iStream& in, DWORD fmt) override {}
    void write(oStream& out, DWORD fmt) const override {}

    virtual void activate(int arg) {}
};
static_assert(sizeof(WorkspaceItemNode) == 0x14, "WorkspaceItemNode size");
