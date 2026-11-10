#include "MsSQLserver.h"
#include "SqlException.h"
#include "Strings.h"
#include "Exception.h"
#include <cwchar>

extern HCURSOR hCursor;
extern HCURSOR g_arrowCursor;
extern HWND    g_hMainWnd;

MsSQLserver::~MsSQLserver()
{
    if (m_dbc)
    {
        SQLDisconnect(m_dbc);
        SQLFreeHandle(SQL_HANDLE_DBC, m_dbc);
        m_dbc = 0;
    }
    if (m_env)
    {
        SQLFreeHandle(SQL_HANDLE_ENV, m_env);
        m_env = 0;
    }
}

HRESULT MsSQLserver::init()
{
    HRESULT hr = 0;
    if (!m_env)
    {
        hr = checkResult(SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_env), 0);
        if (!hr)
            hr = checkResult(SQLSetEnvAttr(m_env, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER), 0);
    }
    if (!m_dbc)
    {
        if (hr)
            goto fail;
        hr = checkResult(SQLAllocHandle(SQL_HANDLE_DBC, m_env, &m_dbc), 0);
        if (hr)
            goto fail;
        hr = checkResult(SQLSetConnectAttrW(m_dbc, 1217, (SQLPOINTER)1, SQL_IS_INTEGER), 0);
        if (hr)
            goto fail;
        hr = checkResult(SQLSetConnectAttrW(m_dbc, 1218, (SQLPOINTER)1, SQL_IS_INTEGER), 0);
    }
    if (!hr)
        return hr;
fail:
    release();
    return hr;
}

void MsSQLserver::release()
{
    if (m_dbc)
    {
        SQLDisconnect(m_dbc);
        SQLFreeHandle(SQL_HANDLE_DBC, m_dbc);
        m_dbc = 0;
    }
    if (m_env)
    {
        SQLFreeHandle(SQL_HANDLE_ENV, m_env);
        m_env = 0;
    }
}

HRESULT MsSQLserver::connect(const wchar_t* driver, const wchar_t* server, int noPrompt,
                             const wchar_t* user, const wchar_t* password)
{
    m_database.clear();
    m_connected = false;
    HRESULT hr = init();
    if (hr)
        return hr;
    String conn;
    conn << L"DRIVER={" << driver << L"};" << L"SERVER=" << server << L";UID=" << user;
    if (password)
        conn << L";PWD=" << password;
    else
        conn << L";trusted_connection=Yes";
    conn << L";APP=shStudio;DATABASE=PS_GameDefs;";
    SetCursor(hCursor);
    SQLRETURN r = SQLDriverConnectW(m_dbc, 0, (SQLWCHAR*)conn.m_buffer, SQL_NTS, 0, 0, 0, SQL_DRIVER_NOPROMPT);
    SetCursor(g_arrowCursor);
    hr = 0;
    if (r == SQL_SUCCESS || r == SQL_SUCCESS_WITH_INFO)
    {
        m_connected = true;
    }
    else if (noPrompt)
    {
        hr = checkResult(r, 0);
    }
    else
    {
        conn.clear();
        conn << L"DRIVER={" << driver << L"};" << L"SERVER=" << server << ';';
        SQLWCHAR out[256];
        memset(out, 0, 0x1FE);
        SQLSMALLINT outLength = 256;
        r = SQLDriverConnectW(m_dbc, g_hMainWnd, (SQLWCHAR*)conn.m_buffer, SQL_NTS, out, 256, &outLength,
                              SQL_DRIVER_PROMPT);
        if (r == 0 || r == 1)
            m_connected = true;
        else
            hr = checkResult(r, 0);
    }
    return hr;
}

HRESULT MsSQLserver::disconnect()
{
    m_connected = false;
    if (m_dbc)
    {
        SQLDisconnect(m_dbc);
        SQLFreeHandle(SQL_HANDLE_DBC, m_dbc);
        m_dbc = 0;
    }
    m_database.clear();
    return 0;
}

HRESULT MsSQLserver::setDatabase(const wchar_t* name)
{
    m_database = name;
    return 0;
}

HRESULT MsSQLserver::getColumns(const wchar_t* table, Columns* out)
{
    out->deleteAll();
    if (!table)
        return 87;
    if (!m_env || !m_dbc)
        return 1062;
    SQLHSTMT stmt = 0;
    HRESULT hr = checkResult(SQLAllocHandle(SQL_HANDLE_STMT, m_dbc, &stmt), 0);
    if (!hr)
    {
        hr = checkResult(SQLColumnsW(stmt, (SQLWCHAR*)m_database.m_buffer, SQL_NTS, (SQLWCHAR*)L"dbo", SQL_NTS,
                                     (SQLWCHAR*)table, SQL_NTS, 0, 0), stmt);
        if (!hr)
        {
            SQLWCHAR name[129];
            SQLWCHAR typeName[129];
            SQLSMALLINT dataType = 0;
            SQLINTEGER bufferLength = 0;
            SQLLEN nameInd = 0, typeInd = 0, typeNameInd = 0, lengthInd = 0;
            SQLBindCol(stmt, 4, SQL_C_CHAR, name, 129, &nameInd);
            SQLBindCol(stmt, 5, SQL_C_SSHORT, &dataType, 0, &typeInd);
            SQLBindCol(stmt, 6, SQL_C_CHAR, typeName, 129, &typeNameInd);
            SQLBindCol(stmt, 8, SQL_C_SLONG, &bufferLength, 0, &lengthInd);
            SQLRETURN r = SQLFetch(stmt);
            if (r == SQL_NO_DATA)
                hr = 87;
            else if (r == 0)
            {
                do
                {
                    bool identity = wcsstr(typeName, L"identity") != 0;
                    Column* column = new Column(name, mapType(dataType), bufferLength, identity);
                    out->add(column);
                    r = SQLFetch(stmt);
                } while (!r);
                hr = (r == SQL_NO_DATA) ? 0 : r;
            }
            else
                hr = r;
        }
    }
    if (stmt)
        SQLFreeStmt(stmt, SQL_DROP);
    return hr;
}

RecordSet MsSQLserver::query(const wchar_t* sql)
{
    MsSqlRecordSet* result = 0;
    SQLHSTMT stmt = 0;
    SQLRETURN r = SQLAllocHandle(SQL_HANDLE_STMT, m_dbc, &stmt);
    if (!stmt)
        r = 8;
    if ((stmt && !r) || !checkResult(r, 0))
    {
        if (!checkResult(SQLPrepareW(stmt, (SQLWCHAR*)sql, SQL_NTS), 0))
        {
            SQLSMALLINT params = -1;
            SQLRETURN pr = SQLNumParams(stmt, &params);
            if (!params && !pr)
            {
                MsSqlRecordSet* rs = new MsSqlRecordSet(this);
                if (rs->open(stmt))
                    delete rs;
                else
                    result = rs;
            }
        }
    }
    return RecordSet(result);
}

HRESULT MsSQLserver::execute(const wchar_t* sql)
{
    SQLHSTMT stmt = 0;
    HRESULT hr = SQLAllocHandle(SQL_HANDLE_STMT, m_dbc, &stmt);
    if (!stmt)
        return 8;
    if (!hr)
        hr = checkResult(SQLExecDirectW(stmt, (SQLWCHAR*)sql, SQL_NTS), stmt);
    if (stmt)
        SQLFreeStmt(stmt, SQL_DROP);
    return hr;
}

HRESULT MsSQLserver::insert(const wchar_t* , int* )
{
    return E_FAIL;
}

HRESULT MsSQLserver::executeUpdate(const wchar_t* sql)
{
    SQLHSTMT stmt = 0;
    HRESULT hr = SQLAllocHandle(SQL_HANDLE_STMT, m_dbc, &stmt);
    if (!stmt)
        return 8;
    if (!hr)
    {
        hr = checkResult(SQLExecDirectW(stmt, (SQLWCHAR*)sql, SQL_NTS), 0);
        if (!hr)
        {
            SQLLEN rows = 0;
            SQLRowCount(stmt, &rows);
            if (rows < 1)
                hr = E_FAIL;
        }
    }
    if (stmt)
        SQLFreeStmt(stmt, SQL_DROP);
    return hr;
}

const wchar_t* MsSQLserver::errorName()
{
    return L"E_FAIL";
}

HRESULT MsSQLserver::checkResult(int sqlReturn, void* handle)
{
    m_error.clear();
    switch (sqlReturn)
    {
    case SQL_INVALID_HANDLE:
        return 6;
    case SQL_ERROR:
    case SQL_SUCCESS_WITH_INFO:
        if (handle)
        {
            SQLWCHAR state[6] = { 0 };
            SQLINTEGER native = 0;
            SQLWCHAR message[256];
            SQLSMALLINT length = 0;
            SQLGetDiagRecW(SQL_HANDLE_STMT, handle, 1, state, &native, message, 256, &length);
            if (length)
                m_error << (const wchar_t*)message;
        }
        return sqlReturn != SQL_SUCCESS_WITH_INFO ? E_FAIL : 0;
    case SQL_SUCCESS:
        return 0;
    case SQL_NO_DATA:
        return 100;
    default:
        return 1066;
    }
}

int MsSQLserver::mapType(int sqlType)
{
    switch (sqlType)
    {
    case SQL_WVARCHAR:
    case SQL_LONGVARCHAR:
    case SQL_CHAR:
    case SQL_VARCHAR:
        return 5;
    case SQL_BIT:
        return 6;
    case SQL_TINYINT:
    case SQL_BIGINT:
    case SQL_NUMERIC:
    case SQL_INTEGER:
    case SQL_SMALLINT:
        return 2;
    case SQL_LONGVARBINARY:
    case SQL_VARBINARY:
    case SQL_BINARY:
        return 7;
    case SQL_DECIMAL:
    case SQL_FLOAT:
    case SQL_REAL:
    case SQL_DOUBLE:
        return 3;
    case 9:
    case 10:
    case 11:
    case SQL_TYPE_DATE:
    case SQL_TYPE_TIME:
    case SQL_TYPE_TIMESTAMP:
        return 4;
    default:
        return 0;
    }
}

Table* MsSQLserver::createTable(const wchar_t* name)
{
    return new MsTable(this, name);
}

HRESULT MsSQLserver::useDatabase(const wchar_t* catalog)
{
    setDatabase(catalog);
    SQLHSTMT stmt = 0;
    SQLWCHAR name[129];
    SQLWCHAR type[129];
    SQLLEN nameInd = 0, typeInd = 0;
    HRESULT hr = checkResult(SQLAllocHandle(SQL_HANDLE_STMT, m_dbc, &stmt), 0);
    if (!hr)
    {
        hr = SQLTablesW(stmt, (SQLWCHAR*)catalog, SQL_NTS, (SQLWCHAR*)L"dbo", SQL_NTS, 0, 0, 0, 0);
        if (!hr)
        {
            hr = SQLBindCol(stmt, 3, SQL_C_CHAR, name, 129, &nameInd);
            if (!hr)
                hr = SQLBindCol(stmt, 4, SQL_C_CHAR, type, 129, &typeInd);
        }
    }
    Strings tables(16);
    if (!hr)
    {
        SQLRETURN r = SQLFetch(stmt);
        if (r == SQL_NO_DATA)
            hr = 87;
        else if (r == 0)
        {
            do
            {
                if (wcscmp(type, L"TABLE") == 0)
                    tables.add((const wchar_t*)name);
                r = SQLFetch(stmt);
            } while (!r);
            hr = (r == SQL_NO_DATA) ? 0 : r;
        }
        else
            hr = r;
    }
    if (stmt)
        SQLFreeStmt(stmt, SQL_DROP);
    if (tables.m_count >= 2)
        tables.sort(0, tables.m_count - 1, true);
    int n = tables.m_count;
    for (int i = 0; i < n; ++i)
    {
        String* table = tables.at(i);
        if (table->m_length == 12 && !_wcsnicmp(table->m_buffer, L"dtproperties", 12))
            continue;
        try
        {
            getTable(table->m_buffer);
        }
        catch (...)
        {
        }
    }
    return hr;
}

MsSqlRecordSet::~MsSqlRecordSet()
{
    if (m_stmt)
        SQLFreeStmt(m_stmt, SQL_CLOSE);
    if (m_iterator)
        delete m_iterator;
    if (m_buffer)
        delete[] m_buffer;
    if (m_names)
        delete[] m_names;
    if (m_types)
        delete[] m_types;
    if (m_sizes)
        delete[] m_sizes;
    if (m_offsets)
        delete[] m_offsets;
    if (m_indicators)
        delete[] m_indicators;
}

RecordSet::iterator* MsSqlRecordSet::getIterator()
{
    if (!m_iterator)
        m_iterator = new MsSqlIterator(this);
    return m_iterator;
}

int MsSqlRecordSet::getCount() const
{
    return (int)m_rowCount;
}

int MsSqlRecordSet::getColumnCount() const
{
    return m_columnCount;
}

int sqlBufferSize(SQLSMALLINT sqlType, SQLULEN columnSize)
{
    switch (sqlType)
    {
    case SQL_BIT:
    case 0:
    case 9:
    case 10:
    case 11:
        return (int)columnSize;
    case SQL_TINYINT:
        return 1;
    case SQL_BIGINT:
    case SQL_NUMERIC:
    case SQL_DECIMAL:
    case SQL_INTEGER:
    case SQL_FLOAT:
        return 4;
    case SQL_SMALLINT:
        return 2;
    case SQL_REAL:
    case SQL_DOUBLE:
        return 8;
    default:
        return (int)columnSize + 1;
    }
}

SQLSMALLINT sqlBindType(SQLSMALLINT sqlType)
{
    switch (sqlType)
    {
    case SQL_WVARCHAR:
    case SQL_WCHAR:
        return SQL_C_WCHAR;
    case SQL_BIT:
    case SQL_LONGVARCHAR:
    case SQL_CHAR:
    case SQL_NUMERIC:
    case SQL_VARCHAR:
        return SQL_C_CHAR;
    case SQL_TINYINT:
        return SQL_C_TINYINT;
    case SQL_BIGINT:
    case SQL_INTEGER:
        return SQL_C_LONG;
    case 0:
        return 0;
    case SQL_DECIMAL:
    case SQL_FLOAT:
    case SQL_REAL:
    case SQL_DOUBLE:
        return SQL_C_DOUBLE;
    case SQL_SMALLINT:
        return SQL_C_SHORT;
    default:
        return sqlType;
    }
}

HRESULT MsSqlRecordSet::open(SQLHSTMT stmt)
{
    m_stmt = stmt;
    HRESULT hr = SQLNumResultCols(stmt, &m_columnCount);
    if (!hr && m_columnCount < 1)
        return 50;
    int n = m_columnCount;
    m_names = new String[n];
    m_types = new SQLSMALLINT[n];
    m_sizes = new int[n];
    m_offsets = new int[n];
    m_indicators = new SQLLEN[n];
    memset(m_types, 0, 2 * m_columnCount);
    memset(m_sizes, 0, 4 * m_columnCount);
    memset(m_offsets, 0, 4 * m_columnCount);
    memset(m_indicators, 0, 4 * m_columnCount);
    int offset = 0;
    if (m_columnCount > 0)
    {
        for (SQLSMALLINT i = 0; i < m_columnCount; ++i)
        {
            m_offsets[i] = offset;
            SQLWCHAR columnName[129];
            SQLSMALLINT nameLength = 0;
            SQLSMALLINT sqlType = 0;
            SQLULEN columnSize = 0;
            SQLSMALLINT scale = 0;
            SQLSMALLINT nullable = 0;
            hr = m_server->checkResult(SQLDescribeColW(stmt, i + 1, columnName, 129, &nameLength, &sqlType,
                                                       &columnSize, &scale, &nullable), 0);
            if (hr)
                return hr;
            m_names[i] = (const wchar_t*)columnName;
            m_types[i] = sqlType;
            m_sizes[i] = sqlBufferSize(sqlType, columnSize);
            offset += m_sizes[i];
        }
    }
    else if (hr)
        return hr;
    m_bufferSize = offset;
    m_buffer = new BYTE[offset];
    memset(m_buffer, 0, m_bufferSize);
    for (int i = 0; i < m_columnCount; ++i)
    {
        SQLRETURN r = SQLBindCol(m_stmt, (SQLUSMALLINT)(i + 1), sqlBindType(m_types[i]), m_buffer + m_offsets[i],
                                 m_sizes[i], &m_indicators[i]);
        if (r)
        {
            hr = m_server->checkResult(r, 0);
            if (hr)
                return hr;
            break;
        }
    }
    hr = m_server->checkResult(SQLExecute(m_stmt), 0);
    if (hr)
        return hr;
    return SQLRowCount(m_stmt, &m_rowCount);
}

int MsSqlRecordSet::findColumn(const wchar_t* name) const
{
    if (!m_stmt || m_columnCount <= 0)
        return -1;
    for (int i = 0; i < m_columnCount; ++i)
    {
        const String& column = m_names[i];
        bool match;
        if (!name || !*name)
            match = column.m_length == 0;
        else if ((int)wcslen(name) == column.m_length)
            match = _wcsnicmp(column.m_buffer, name, column.m_length) == 0;
        else
            match = false;
        if (match)
            return i;
    }
    return -1;
}

bool MsSqlIterator::atEnd() const
{
    return m_row >= m_rs->m_rowCount;
}

int MsSqlIterator::getRow() const
{
    return m_row;
}

bool MsSqlIterator::next()
{
    MsSqlRecordSet* rs = m_rs;
    SQLLEN count = rs->m_rowCount;
    if (count != -1 && m_row >= count)
        return false;
    ++m_row;
    memset(rs->m_buffer, 0, rs->m_bufferSize);
    return SQLFetch(m_rs->m_stmt) == 0;
}

static void p02_checkColumn(const MsSqlRecordSet* rs, int index)
{
    if (index < 0 || index >= rs->m_columnCount)
        throw ArrayIndexOutOfBoundsException(index, rs->m_columnCount);
}

static int p02_columnIndex(const MsSqlRecordSet* rs, const wchar_t* column)
{
    int index = rs->findColumn(column);
    if (index == -1)
        throw SqlException(L"Invalid column name");
    return index;
}

const wchar_t* MsSqlIterator::getString(const wchar_t* column)
{
    return getStringAt(p02_columnIndex(m_rs, column));
}

const wchar_t* MsSqlIterator::getStringAt(int index)
{
    p02_checkColumn(m_rs, index);
    return (const wchar_t*)(m_rs->m_buffer + m_rs->m_offsets[index]);
}

int MsSqlIterator::getInt(const wchar_t* column)
{
    return getIntAt(p02_columnIndex(m_rs, column));
}

int MsSqlIterator::getIntAt(int index)
{
    p02_checkColumn(m_rs, index);
    switch (m_rs->m_types[index])
    {
    case SQL_TINYINT:
    case SQL_BIGINT:
    case SQL_NUMERIC:
    case SQL_INTEGER:
    case SQL_SMALLINT:
    {
        __int64 value = 0;
        const BYTE* data = m_rs->m_buffer + m_rs->m_offsets[index];
        int size = m_rs->m_sizes[index];
        if (size > 0)
            memcpy(&value, data, size);
        return (int)value;
    }
    default:
        throw SqlCoercionException(L"column can not be coerced to integer value");
    }
}

double MsSqlIterator::getDouble(const wchar_t* column)
{
    return getDoubleAt(p02_columnIndex(m_rs, column));
}

double MsSqlIterator::getDoubleAt(int index)
{
    p02_checkColumn(m_rs, index);
    int type = m_rs->m_types[index];
    if (type != SQL_DECIMAL && (type <= SQL_SMALLINT || type > SQL_DOUBLE))
        throw SqlCoercionException(L"column can not be coerced to float value");
    const BYTE* data = m_rs->m_buffer + m_rs->m_offsets[index];
    int size = m_rs->m_sizes[index];
    if (size == 4)
        return *(const float*)data;
    if (size == 8)
        return *(const double*)data;
    return 0.0;
}

SYSTEMTIME MsSqlIterator::getTimeAt(int index)
{
    p02_checkColumn(m_rs, index);
    if ((unsigned)(m_rs->m_types[index] - SQL_TYPE_DATE) > 2)
        throw SqlCoercionException(L"Column can not be coerced to date value");
    SYSTEMTIME result;
    memset(&result, 0, sizeof(result));
    return result;
}

bool MsSqlIterator::hasColumn(const wchar_t* column)
{
    return m_rs->findColumn(column) >= 0;
}

void MsSqlIterator::vslot12(String& out)
{
    out.clear();
}
