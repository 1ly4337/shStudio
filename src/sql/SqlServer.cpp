#include "SqlServer.h"
#include "Table.h"
#include "RecordSet.h"
#include "SqlException.h"
#include <cstdarg>
#include <cstdio>

SqlException::SqlException(const wchar_t* fmt, ...)
{
    wchar_t buffer[0x200];
    memset(buffer, 0, sizeof(buffer));
    va_list args;
    va_start(args, fmt);
    _vsnwprintf_s(buffer, 0x200, 0x200, fmt, args);
    va_end(args);
    format(buffer);
}

RecordSet::~RecordSet()
{
    release();
}

HRESULT SqlServer::disconnect()
{
    m_connected = false;
    return 0;
}

Table* SqlServer::getTable(int index)
{
    Table* table;
    if (index < 0 || index >= m_tables.m_count || (table = m_tables.m_items[index].value) == 0)
        throw SqlException(L"Table does not exist");
    return table;
}

Table* SqlServer::getTable(const wchar_t* name)
{
    associativeArray<Table*>::Item* item = m_tables.find(name);
    Table* table;
    if (item && (table = item->value) != 0)
        return table;
    table = createTable(name);
    if (table)
    {
        if (table->m_valid)
        {
            m_tables.set(name, table);
            return table;
        }
        delete table;
    }
    throw SqlException(L"Table does not exist");
}

Table* SqlServer::createTable(const wchar_t* name)
{
    return new Table(this, name);
}
