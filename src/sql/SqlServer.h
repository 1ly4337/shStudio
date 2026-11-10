#pragma once
#include <windows.h>
#include "String.hpp"
#include "associativeArray.h"
#include "RecordSet.h"
#include "Table.h"

class Table;
class Columns;

class SqlServer
{
public:
    SqlServer() : m_tables(16), m_connected(false) {}
    virtual ~SqlServer() {}

    virtual HRESULT init() { return 0; }
    virtual void release() {}
    virtual HRESULT connect(const wchar_t* driver, const wchar_t* server, int noPrompt,
                            const wchar_t* user, const wchar_t* password) = 0;
    virtual HRESULT disconnect();
    virtual Table* getTable(const wchar_t* name);
    virtual Table* getTable(int index);
    virtual HRESULT setDatabase(const wchar_t* name) = 0;
    virtual HRESULT getColumns(const wchar_t* table, Columns* out) = 0;
    virtual RecordSet query(const wchar_t* sql) = 0;
    virtual HRESULT execute(const wchar_t* sql) = 0;
    virtual HRESULT insert(const wchar_t* sql, int* newId) = 0;
    virtual HRESULT executeUpdate(const wchar_t* sql) = 0;
    virtual const wchar_t* errorName() = 0;
    virtual HRESULT checkResult(int sqlReturn, void* handle) = 0;
    virtual int mapType(int sqlType) = 0;
    virtual Table* createTable(const wchar_t* name);

    associativeArray<Table*> m_tables;
    bool                     m_connected;
    String                   m_error;
};
static_assert(sizeof(SqlServer) == 0x3C, "SqlServer size");
