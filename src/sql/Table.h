#pragma once
#include <windows.h>
#include "String.hpp"
#include "ptrArray.h"
#include "RecordSet.h"

class SqlServer;
class Tokeniser;

class Column
{
public:
    Column(const wchar_t* name, int type, int size, bool identity);
    virtual ~Column() {}

    String m_name;
    int    m_type;
    int    m_size;
    bool   m_identity;
};
static_assert(sizeof(Column) == 0x30, "Column size");

class Columns : public ptrArray<Column*>
{
public:
    Columns() : ptrArray<Column*>(256) {}
    virtual ~Columns() {}

    Column* find(const wchar_t* name) const;

    Column* at(int index) const;
};
static_assert(sizeof(Columns) == 0x18, "Columns size");

class Table;

struct SqlJoin
{
    Column* check(Table* main);
    String fieldList() const;

    void*  m_unk00;
    Table* m_table;
    String m_key;
    String m_fields;
    String m_type;
    BYTE   m_alias;
};
static_assert(sizeof(SqlJoin) == 0x6C, "SqlJoin size");

struct SqlJoins : public ptrArray<SqlJoin*>
{
    void check(Table* main);
    String fieldList() const;
};

String& appendJoin(String& out, const SqlJoin* join);
String& appendJoins(String& out, const SqlJoins* joins);

struct SqlRow
{
    String m_names;
    String m_values;
    String m_assignments;
};
static_assert(sizeof(SqlRow) == 0x60, "SqlRow size");

class Table
{
public:
    Table(SqlServer* server, const wchar_t* name);
    virtual ~Table();

    virtual String getFullName() const;
    virtual RecordSet selectById(const wchar_t* fields, long id);
    virtual RecordSet selectJoins(const wchar_t* fields, SqlJoins* joins, const wchar_t* orderBy);
    virtual RecordSet selectJoin(const wchar_t* fields, SqlJoin* join, const wchar_t* orderBy);
    virtual RecordSet select(const wchar_t* fields, const wchar_t* orderBy);
    virtual RecordSet selectJoinsWhere(const wchar_t* fields, SqlJoins* joins, const wchar_t* where,
                                       const wchar_t* orderBy);
    virtual RecordSet selectJoinWhere(const wchar_t* fields, SqlJoin* join, const wchar_t* where,
                                      const wchar_t* orderBy);
    virtual RecordSet selectWhere(const wchar_t* fields, const wchar_t* where, const wchar_t* orderBy);
    virtual HRESULT deleteWhere(const wchar_t* where);
    virtual HRESULT insert(const wchar_t* fields, const wchar_t* values);
    virtual HRESULT insertRow(const SqlRow& row);
    virtual HRESULT insertBlob(const wchar_t* fields, const void* data, int size, const wchar_t* values);
    virtual HRESULT update(const wchar_t* fields, const wchar_t* values, long id);
    virtual HRESULT updateRow(const SqlRow& row, long id);
    virtual HRESULT updateBlob(const wchar_t* field, const void* data, int size, long id);
    virtual HRESULT insertValues(const wchar_t* fields, ...);
    virtual HRESULT updateValues(long id, const wchar_t* fields, ...);
    virtual int getLastId() const;

    SqlServer* m_server;
    String     m_name;
    Columns    m_columns;
    bool       m_valid;
    int        m_lastId;

protected:
    const wchar_t* keyColumnName() const;
};
static_assert(sizeof(Table) == 0x48, "Table size");

class MsTable : public Table
{
public:
    MsTable(SqlServer* server, const wchar_t* name) : Table(server, name) {}
    virtual ~MsTable() {}

    String getFullName() const override;

    mutable String m_fullName;
};
static_assert(sizeof(MsTable) == 0x68, "MsTable size");

const wchar_t* sqlNextToken(Tokeniser& tokens);
