#pragma once
#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include "SqlServer.h"
#include "Table.h"

class MsSQLserver : public SqlServer
{
public:
    MsSQLserver() : m_env(0), m_dbc(0) {}
    virtual ~MsSQLserver();

    HRESULT init() override;
    void release() override;
    HRESULT connect(const wchar_t* driver, const wchar_t* server, int noPrompt,
                    const wchar_t* user, const wchar_t* password) override;
    HRESULT disconnect() override;
    HRESULT setDatabase(const wchar_t* name) override;
    HRESULT getColumns(const wchar_t* table, Columns* out) override;
    RecordSet query(const wchar_t* sql) override;
    HRESULT execute(const wchar_t* sql) override;
    HRESULT insert(const wchar_t* sql, int* newId) override;
    HRESULT executeUpdate(const wchar_t* sql) override;
    const wchar_t* errorName() override;
    HRESULT checkResult(int sqlReturn, void* handle) override;
    int mapType(int sqlType) override;
    Table* createTable(const wchar_t* name) override;
    virtual HRESULT useDatabase(const wchar_t* catalog);

    SQLHENV m_env;
    SQLHDBC m_dbc;
    String  m_database;
};
static_assert(sizeof(MsSQLserver) == 0x64, "MsSQLserver size");

class MsSqlIterator;

class MsSqlRecordSet : public RecordSet::_RecordSet
{
public:
    explicit MsSqlRecordSet(MsSQLserver* server)
        : m_server(server), m_stmt(0), m_iterator(0), m_columnCount(0), m_rowCount(0), m_names(0), m_types(0),
          m_sizes(0), m_indicators(0), m_offsets(0), m_buffer(0), m_bufferSize(0) {}
    virtual ~MsSqlRecordSet();

    RecordSet::iterator* getIterator() override;
    int getCount() const override;
    int getColumnCount() const override;

    HRESULT open(SQLHSTMT stmt);
    int findColumn(const wchar_t* name) const;

    MsSQLserver*   m_server;
    SQLHSTMT       m_stmt;
    MsSqlIterator* m_iterator;
    SQLSMALLINT    m_columnCount;
    SQLLEN         m_rowCount;
    String*        m_names;
    SQLSMALLINT*   m_types;
    int*           m_sizes;
    SQLLEN*        m_indicators;
    int*           m_offsets;
    BYTE*          m_buffer;
    int            m_bufferSize;
};
static_assert(sizeof(MsSqlRecordSet) == 0x38, "MsSqlRecordSet size");

class MsSqlIterator : public RecordSet::iterator
{
public:
    explicit MsSqlIterator(MsSqlRecordSet* rs) : m_rs(rs), m_row(0) {}

    bool atEnd() const override;
    int  getRow() const override;
    bool next() override;
    const wchar_t* getString(const wchar_t* column) override;
    const wchar_t* getStringAt(int index) override;
    int  getInt(const wchar_t* column) override;
    int  getIntAt(int index) override;
    double getDouble(const wchar_t* column) override;
    double getDoubleAt(int index) override;
    SYSTEMTIME getTimeAt(int index) override;
    bool hasColumn(const wchar_t* column) override;
    void vslot12(String& out) override;

    MsSqlRecordSet* m_rs;
    int             m_row;
};
static_assert(sizeof(MsSqlIterator) == 0xC, "MsSqlIterator size");

int sqlBufferSize(SQLSMALLINT sqlType, SQLULEN columnSize);
SQLSMALLINT sqlBindType(SQLSMALLINT sqlType);
