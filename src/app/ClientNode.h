#pragma once
#include <windows.h>
#include "WorkspaceDataSet.h"
#include "String.hpp"
#include "ClientData.h"

class TreeList;

#pragma vtordisp(push, 2)
class ClientNode : public WorkspaceDataSet
{
public:
    ClientNode(TreeList* tree, int unused);
    virtual ~ClientNode();

    const wchar_t* getString(DWORD tag) const override;
    void onInitItemMenu(HMENU menu, HWND tree, HTREEITEM item) override;
    void unload() override;
    bool open(const String& path, int format) override;
    void refresh(TreeList* tree) override;

    bool createWorkspace();

    String     m_name;
    ClientData m_clientData;
};
#pragma vtordisp(pop)
static_assert(sizeof(ClientNode) == 0x33E0, "ClientNode size");
