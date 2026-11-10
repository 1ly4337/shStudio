#pragma once
#include <windows.h>
#include "primitiveArray.h"
#include "Strings.h"

class Table;
class ListCtrl;

enum SqlColumnFlags
{
    SQLCOL_TEXT    = 0x1000000,
    SQLCOL_TIME    = 0x2000000,
    SQLCOL_INT     = 0x4000000,
    SQLCOL_INDEX   = 0x0FFFFFF,
};

class SQLSchema
{
public:
    SQLSchema();
    virtual ~SQLSchema();

    int build(Table* table, ListCtrl* list);

    bool hasFlag(int index, long flag) { return index < m_columnCount && (*m_types.at(index) & flag) != 0; }

public:
    int  m_textCount;
    int  m_numberCount;
    int  m_columnCount;
    primitiveArray<long> m_types;
    Strings m_names;
};
static_assert(sizeof(SQLSchema) == 0x4C, "SQLSchema size");
