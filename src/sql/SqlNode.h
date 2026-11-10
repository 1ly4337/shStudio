#pragma once
#include <windows.h>
#include "Node.h"
#include "String.hpp"

class SQLSchema;
class RecordSet;

class SqlNode : public Node
{
public:
    explicit SqlNode(SQLSchema* schema);
    virtual ~SqlNode();

    const wchar_t* getString(DWORD tag) const override;
    int getInt(DWORD tag) const override;
    void reset() override;
    int compare(const Node* other, DWORD tag) const override;

    void load(RecordSet& rs);

public:
    SQLSchema* m_schema;
    String*    m_strings;
    int*       m_values;
};
static_assert(sizeof(SqlNode) == 0x1C, "SqlNode size");
