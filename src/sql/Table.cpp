#include "Table.h"
#include "SqlServer.h"
#include "MsSQLserver.h"
#include "SqlException.h"
#include "Tokeniser.h"
#include "Exception.h"
#include <cwchar>
#include <cstdarg>

const wchar_t* sqlNextToken(Tokeniser& t)
{
    if (!t.m_text || t.m_current >= t.m_length)
        return 0;
    if (t.m_current < 0)
    {
        t.m_current = 0;
        return t.m_text;
    }
    const wchar_t* p = t.m_text + t.m_current;
    while (t.m_current < t.m_length && *p)
    {
        ++t.m_current;
        ++p;
    }
    if (t.m_current == t.m_length)
        return 0;
    ++t.m_current;
    return p + 1;
}

Column::Column(const wchar_t* name, int type, int size, bool identity)
{
    m_name = name;
    m_type = type;
    m_size = size;
    m_identity = identity;
}

Column* Columns::find(const wchar_t* name) const
{
    for (int i = 0; i < m_count; ++i)
    {
        Column* c = m_items[i];
        bool match;
        if (!name || !*name)
            match = c->m_name.m_length == 0;
        else if ((int)wcslen(name) == c->m_name.m_length)
            match = _wcsnicmp(c->m_name.m_buffer, name, c->m_name.m_length) == 0;
        else
            match = false;
        if (match)
            return m_items[i];
    }
    return 0;
}

Column* Columns::at(int index) const
{
    if (index < 0 || index >= m_count)
        throw ArrayIndexOutOfBoundsException(index, m_count);
    return m_items[index];
}

Column* SqlJoin::check(Table* main)
{
    if (m_table == main)
        throw SqlException(L"Table joined on itself");
    Column* key;
    if (!m_key.m_length || !(key = main->m_columns.find(m_key.m_buffer)))
        throw SqlException(L"Invalid join key");
    return key;
}

String SqlJoin::fieldList() const
{
    String out;
    Tokeniser tokens;
    tokens.set(m_fields.m_buffer);
    for (int i = 0; i < tokens.m_count; ++i)
    {
        const wchar_t* field = sqlNextToken(tokens);
        if (!field)
            break;
        out << L",t" << (unsigned char)m_alias << L'.' << field;
    }
    return out;
}

void SqlJoins::check(Table* main)
{
    for (int i = 0; i < m_count; ++i)
    {
        SqlJoin* join = m_items[i];
        if (join->m_table == main)
            throw SqlException(L"Table joined on itself");
        if (!join->m_key.m_length || !main->m_columns.find(join->m_key.m_buffer))
            throw SqlException(L"Invalid join key");
    }
}

String SqlJoins::fieldList() const
{
    String out;
    for (int i = 0; i < m_count; ++i)
        out << m_items[i]->fieldList();
    return out;
}

String& appendJoin(String& out, const SqlJoin* join)
{
    const wchar_t* table = join->m_table->m_name.m_buffer;
    out << L' ' << join->m_type << L" JOIN " << table << L" t" << (unsigned char)join->m_alias << L" on t"
        << (unsigned char)join->m_alias << L'.' << join->m_key << L"=t1." << join->m_key;
    return out;
}

String& appendJoins(String& out, const SqlJoins* joins)
{
    for (int i = 0; i < joins->m_count; ++i)
        appendJoin(out, joins->m_items[i]);
    return out;
}

Table::Table(SqlServer* server, const wchar_t* name)
{
    m_server = server;
    m_name.append(name);
    m_valid = m_server->getColumns(m_name.m_buffer, &m_columns) == 0;
    m_lastId = -1;
}

Table::~Table()
{
}

String Table::getFullName() const
{
    return String(m_name);
}

int Table::getLastId() const
{
    return m_lastId;
}

const wchar_t* Table::keyColumnName() const
{
    for (int i = 0; i < m_columns.m_count; ++i)
    {
        if (m_columns.m_items[i]->m_identity)
            return m_columns.m_items[i]->m_name.m_buffer;
    }
    return 0;
}

RecordSet Table::selectById(const wchar_t* fields, long id)
{
    wchar_t where[32];
    memset(where, 0, sizeof(where));
    swprintf_s(where, 32, L"%s=%ld", keyColumnName(), id);
    return selectWhere(fields, where, 0);
}

RecordSet Table::select(const wchar_t* fields, const wchar_t* orderBy)
{
    m_lastId = -1;
    if (!fields)
        throw SqlException(L"Missing list of fields");
    String sql;
    {
        String name = getFullName();
        sql << L"SELECT " << fields << L" FROM " << name;
    }
    if (orderBy)
        sql << L" ORDER BY " << orderBy;
    return m_server->query(sql.m_buffer);
}

RecordSet Table::selectWhere(const wchar_t* fields, const wchar_t* where, const wchar_t* orderBy)
{
    m_lastId = -1;
    if (!fields || !where)
        throw SqlException(L"Invalid arguments");
    String sql;
    {
        String name = getFullName();
        sql << L"SELECT " << fields << L" FROM " << name << L" WHERE " << where;
    }
    if (orderBy)
        sql << L" ORDER BY " << orderBy;
    return m_server->query(sql.m_buffer);
}

static void p02_mainFields(String& out, const wchar_t* fields)
{
    Tokeniser tokens;
    tokens.set(fields);
    for (int i = 0; i < tokens.m_count; ++i)
    {
        const wchar_t* field = sqlNextToken(tokens);
        if (!field)
            break;
        if (out.m_length)
            out << L',';
        out << L"t1." << field;
    }
}

RecordSet Table::selectJoin(const wchar_t* fields, SqlJoin* join, const wchar_t* orderBy)
{
    m_lastId = -1;
    if (!fields)
        throw SqlException(L"Missing list of fields");
    join->check(this);
    String list;
    p02_mainFields(list, fields);
    list << join->fieldList();
    String sql;
    sql << L"SELECT " << list << L" FROM " << m_name << L" t1";
    appendJoin(sql, join);
    if (orderBy)
        sql << L" ORDER BY " << orderBy;
    return m_server->query(sql.m_buffer);
}

RecordSet Table::selectJoins(const wchar_t* fields, SqlJoins* joins, const wchar_t* orderBy)
{
    m_lastId = -1;
    if (!fields)
        throw SqlException(L"Missing list of fields");
    joins->check(this);
    String list;
    p02_mainFields(list, fields);
    list << joins->fieldList();
    String sql;
    sql << L"SELECT " << list << L" FROM " << m_name << L" t1 ";
    appendJoins(sql, joins);
    if (orderBy)
        sql << L" ORDER BY " << orderBy;
    return m_server->query(sql.m_buffer);
}

RecordSet Table::selectJoinWhere(const wchar_t* fields, SqlJoin* join, const wchar_t* where, const wchar_t* orderBy)
{
    m_lastId = -1;
    if (!fields)
        throw SqlException(L"Missing list of fields");
    join->check(this);
    String list;
    p02_mainFields(list, fields);
    list << join->fieldList();
    String sql;
    sql << L"SELECT " << list << L" FROM " << m_name << L" t1";
    appendJoin(sql, join);
    if (where)
        sql << L" WHERE " << where;
    if (orderBy)
        sql << L" ORDER BY " << orderBy;
    return m_server->query(sql.m_buffer);
}

RecordSet Table::selectJoinsWhere(const wchar_t* fields, SqlJoins* joins, const wchar_t* where, const wchar_t* orderBy)
{
    m_lastId = -1;
    if (!fields)
        throw SqlException(L"Missing list of fields");
    joins->check(this);
    String list;
    p02_mainFields(list, fields);
    list << joins->fieldList();
    String sql;
    sql << L"SELECT " << list << L" FROM " << m_name << L" t1 ";
    appendJoins(sql, joins);
    if (where)
        sql << L" WHERE " << where;
    if (orderBy)
        sql << L" ORDER BY " << orderBy;
    return m_server->query(sql.m_buffer);
}

HRESULT Table::deleteWhere(const wchar_t* where)
{
    m_lastId = -1;
    if (!where)
        throw SqlException(L"Invalid arguments");
    String sql;
    sql << L"DELETE FROM " << m_name << L" WHERE " << where;
    return m_server->execute(sql.m_buffer);
}

HRESULT Table::insert(const wchar_t* fields, const wchar_t* values)
{
    m_lastId = -1;
    if (!fields || !values)
        return 13;
    String sql;
    sql << L"INSERT INTO " << m_name << L" (" << fields << L") VALUES (" << values << L')';
    return m_server->insert(sql.m_buffer, &m_lastId);
}

HRESULT Table::insertRow(const SqlRow& row)
{
    m_lastId = -1;
    if (!row.m_names.m_length)
        return 13;
    String sql;
    sql << L"INSERT INTO " << m_name << L" (" << row.m_names << L") VALUES (" << row.m_values << L')';
    return m_server->insert(sql.m_buffer, &m_lastId);
}

HRESULT Table::insertBlob(const wchar_t* fields, const void* data, int , const wchar_t* values)
{
    m_lastId = -1;
    if (!fields || !data)
        return 13;
    String sql;
    sql << L"INSERT INTO " << m_name << L" (" << fields << L") VALUES (";
    if (values)
        sql << values << ',';
    sql << L"?)";
    return E_FAIL;
}

HRESULT Table::update(const wchar_t* fields, const wchar_t* values, long id)
{
    const wchar_t* key = keyColumnName();
    if (!key)
        return 50;
    static const wchar_t kDestroyedQuotedValue[] = { 0x78B8, 0x0054, 0 };
    String set;
    Tokeniser fieldTokens;
    Tokeniser valueTokens;
    fieldTokens.set(fields);
    valueTokens.set(values);
    for (;;)
    {
        const wchar_t* field = sqlNextToken(fieldTokens);
        const wchar_t* value = sqlNextToken(valueTokens);
        if (!field)
            break;
        if (set.m_length)
            set << L',';
        Column* c = m_columns.find(field);
        if (c && c->m_type > 3)
        {
            String quoted = String::sqlQuote(value);
            value = kDestroyedQuotedValue;
        }
        set << field << L'=' << value;
    }
    if (!set.m_length)
        return 13;
    String sql;
    sql << L"UPDATE " << m_name << L" SET " << set << L" WHERE " << key << L'=' << id;
    return m_server->executeUpdate(sql.m_buffer);
}

HRESULT Table::updateRow(const SqlRow& row, long id)
{
    const wchar_t* key = keyColumnName();
    if (!key)
        return 50;
    if (!row.m_assignments.m_length)
        return 13;
    String sql;
    sql << L"UPDATE " << m_name << L" SET " << row.m_assignments << L" WHERE " << key << L'=' << id;
    return m_server->executeUpdate(sql.m_buffer);
}

HRESULT Table::updateBlob(const wchar_t* field, const void* data, int , long id)
{
    const wchar_t* key = keyColumnName();
    if (!key)
        return 50;
    if (!field)
        return 13;
    String sql;
    sql << L"UPDATE " << m_name << L" SET " << field << L'=';
    if (data)
        sql << L'?';
    else
        sql << L"null";
    sql << L" WHERE " << key << L'=' << id;
    if (data)
        return E_FAIL;
    return m_server->executeUpdate(sql.m_buffer);
}

static bool p02_appendValue(String& out, const Column* c, va_list& args)
{
    switch (c->m_type)
    {
    case 2:
        switch (c->m_size)
        {
        case 1: out << (unsigned char)va_arg(args, int); return true;
        case 2: out << (unsigned short)va_arg(args, int); return true;
        case 4: out << (long)va_arg(args, long); return true;
        default: return false;
        }
    case 5:
    {
        String quoted = String::sqlQuote(va_arg(args, const wchar_t*));
        out.append(quoted.m_buffer, quoted.m_length);
        return true;
    }
    default:
        return false;
    }
}

HRESULT Table::insertValues(const wchar_t* fields, ...)
{
    m_lastId = -1;
    if (!fields)
        return 13;
    String names;
    String values;
    Tokeniser tokens;
    tokens.set(fields);
    va_list args;
    va_start(args, fields);
    for (int i = 0; i < tokens.m_count; ++i)
    {
        const wchar_t* field = sqlNextToken(tokens);
        Column* c = m_columns.find(field);
        if (!c)
        {
            va_end(args);
            return 0x16;
        }
        if (names.m_length)
            names << L',';
        names << field;
        if (values.m_length)
            values << L',';
        if (!p02_appendValue(values, c, args))
        {
            va_end(args);
            return 50;
        }
    }
    va_end(args);
    String sql;
    sql << L"INSERT INTO " << m_name << L" (" << names << L") VALUES (" << values << L')';
    return m_server->insert(sql.m_buffer, &m_lastId);
}

HRESULT Table::updateValues(long id, const wchar_t* fields, ...)
{
    if (!fields)
        return 13;
    const wchar_t* key = keyColumnName();
    if (!key)
        return 50;
    String set;
    Tokeniser tokens;
    tokens.set(fields);
    va_list args;
    va_start(args, fields);
    for (int i = 0; i < tokens.m_count; ++i)
    {
        if (set.m_length)
            set << L',';
        const wchar_t* field = sqlNextToken(tokens);
        set << field << L'=';
        Column* c = m_columns.find(field);
        if (!c)
        {
            va_end(args);
            return 0x16;
        }
        if (!p02_appendValue(set, c, args))
        {
            va_end(args);
            return 50;
        }
    }
    va_end(args);
    String sql;
    sql << L"UPDATE " << m_name << L" SET " << set << L" WHERE " << key << L'=' << id;
    return m_server->executeUpdate(sql.m_buffer);
}

String MsTable::getFullName() const
{
    if (!m_fullName.m_length)
    {
        const MsSQLserver* server = static_cast<const MsSQLserver*>(m_server);
        m_fullName << L'[' << server->m_database << L"].[dbo].[" << m_name << L']';
    }
    return String(m_fullName);
}
