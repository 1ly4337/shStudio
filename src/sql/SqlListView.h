#pragma once
#include <windows.h>
#include "ListCtrlOD.h"
#include "SQLSchema.h"
#include "SqlNodes.h"

class Table;

class SqlListView : public ListCtrlOD
{
public:
    explicit SqlListView(Table* table) : m_table(table) {}
    virtual ~SqlListView();

    DWORD getStyle() override { return 0x10001009; }
    bool onCreate() override;
    Node* getNode(int index) override;
    bool sort() override;
    int indexOfNode(Node* node) override;
    Node* findNode(Node* like) override { return 0; }

    virtual bool loadRows(const wchar_t* orderBy);

public:
    SQLSchema m_schema;
    Table*    m_table;
    SqlNodes  m_nodes;
};
static_assert(sizeof(SqlListView) == 0x15C, "SqlListView size");
