#pragma once
#include <windows.h>
#include "WorkspaceItemNode.h"

class Table;

class TableNode : public WorkspaceItemNode
{
public:
    explicit TableNode(Table* table);
    virtual ~TableNode();

    int getId() const override { return 19; }
    const wchar_t* getString(DWORD tag) const override;
    int getInt(DWORD tag) const override;
    void reset() override {}
    int compare(const Node*, DWORD) const override { return 0; }
    void activate(int arg) override;

public:
    Table* m_table;
};
static_assert(sizeof(TableNode) == 0x18, "TableNode size");
