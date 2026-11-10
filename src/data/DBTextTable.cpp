#include "DBTextTable.h"
#include "iStream.hpp"
#include "oStream.hpp"
#include "aStream.h"
#include <string.h>
#include <wchar.h>
#include <stdlib.h>
#include <errno.h>

static const int kHeaderSize = 0x80;
static const unsigned kMaxString = 0x100000;
static const unsigned kMaxColumns = 256;

static unsigned rdDword(const BYTE* p) { return p[0] | (p[1] << 8) | (p[2] << 16) | ((unsigned)p[3] << 24); }
static long long rdInt64(const BYTE* p) { long long v; memcpy(&v, p, 8); return v; }
static void putDword(std::string& s, unsigned v) { s.append((const char*)&v, 4); }
static void putInt64(std::string& s, long long v) { s.append((const char*)&v, 8); }

static bool endsWithNoCase(const std::wstring& s, const wchar_t* tail)
{
    size_t n = wcslen(tail);
    return s.size() >= n && !_wcsicmp(s.c_str() + s.size() - n, tail);
}

static bool numericName(const std::wstring& name)
{
    return endsWithNoCase(name, L"id") || endsWithNoCase(name, L"type") || endsWithNoCase(name, L"level")
        || endsWithNoCase(name, L"code");
}

void DBTextTable::clear()
{
    m_prefix.clear();
    m_columns.clear();
    m_keyCols.clear();
    m_strCols.clear();
    m_rows.clear();
    m_split = kNone;
    m_loaded = false;
}

UINT DBTextTable::codePageForName(const wchar_t* fileName)
{
    if (!fileName)
        return 1252;
    const wchar_t* base = wcsrchr(fileName, L'\\');
    base = base ? base + 1 : fileName;
    const wchar_t* lang = 0;
    for (const wchar_t* p = base; *p; ++p)
        if (!_wcsnicmp(p, L"Text_", 5))
            lang = p + 5;
    if (!lang)
        return 1252;
    std::wstring tok;
    for (const wchar_t* p = lang; *p && *p != L'_' && *p != L'.'; ++p)
        tok += *p;
    static const struct { const wchar_t* code; UINT cp; } map[] = {
        { L"CHN", 936 }, { L"CN", 936 }, { L"KOR", 949 }, { L"KR", 949 }, { L"JPN", 932 }, { L"JP", 932 },
        { L"TWN", 950 }, { L"TW", 950 }, { L"RUS", 1251 }, { L"RU", 1251 },
        { L"POL", 1250 }, { L"PL", 1250 },
        { L"TUR", 1254 }, { L"TR", 1254 },
    };
    for (const auto& m : map)
        if (!_wcsicmp(tok.c_str(), m.code))
            return m.cp;
    return 1252;
}

bool DBTextTable::tryLayout(const BYTE* data, int size, int pos, int nKeys, int nStrs, std::vector<Row>& rows) const
{
    rows.clear();
    if (nKeys + nStrs <= 0 || pos + 4 > size)
        return false;
    unsigned count = rdDword(data + pos);
    pos += 4;
    unsigned minRow = 8u * nKeys + 4u * nStrs;
    if ((unsigned long long)count * minRow > (unsigned)(size - pos))
        return false;
    rows.reserve(count);
    for (unsigned r = 0; r < count; ++r)
    {
        Row row;
        row.keys.resize(nKeys);
        for (int k = 0; k < nKeys; ++k)
        {
            if (pos + 8 > size)
                return false;
            row.keys[k] = rdInt64(data + pos);
            pos += 8;
        }
        row.strs.resize(nStrs);
        for (int s = 0; s < nStrs; ++s)
        {
            if (pos + 4 > size)
                return false;
            unsigned len = rdDword(data + pos);
            pos += 4;
            if (len > kMaxString || len > (unsigned)(size - pos))
                return false;
            row.strs[s].assign((const char*)data + pos, len);
            pos += (int)len;
        }
        rows.push_back(std::move(row));
    }
    return pos == size;
}

bool DBTextTable::parse(const BYTE* data, int size)
{
    clear();
    m_error.clear();
    if (!data || size < kHeaderSize + 8)
    {
        m_error = L"The file is too small to be a DB text table.";
        return false;
    }
    int pos = kHeaderSize;
    unsigned ncols = rdDword(data + pos);
    pos += 4;
    if (!ncols || ncols > kMaxColumns)
    {
        m_error = L"Invalid column count in the DB text table header.";
        return false;
    }
    std::vector<std::wstring> cols;
    for (unsigned c = 0; c < ncols; ++c)
    {
        if (pos + 1 > size)
        {
            m_error = L"Truncated column list.";
            return false;
        }
        int chars = data[pos++];
        if (pos + 2 * chars > size)
        {
            m_error = L"Truncated column list.";
            return false;
        }
        std::wstring name;
        for (int i = 0; i < chars; ++i)
            name += (wchar_t)(data[pos + 2 * i] | (data[pos + 2 * i + 1] << 8));
        pos += 2 * chars;
        cols.push_back(name);
    }

    struct Candidate { Split split; std::vector<int> keys, strs; };
    std::vector<Candidate> cands;
    {
        Candidate c{ kByName, {}, {} };
        for (unsigned i = 0; i < ncols; ++i)
            (numericName(cols[i]) ? c.keys : c.strs).push_back((int)i);
        cands.push_back(c);
    }
    {
        Candidate c{ kImplicitKey, { -1 }, {} };
        for (unsigned i = 0; i < ncols; ++i)
            c.strs.push_back((int)i);
        cands.push_back(c);
    }
    for (unsigned k = 0; k <= ncols; ++k)
    {
        Candidate c{ kLeadingKeys, {}, {} };
        for (unsigned i = 0; i < ncols; ++i)
            (i < k ? c.keys : c.strs).push_back((int)i);
        cands.push_back(c);
    }

    std::vector<Row> rows;
    for (const Candidate& c : cands)
    {
        if (!tryLayout(data, size, pos, (int)c.keys.size(), (int)c.strs.size(), rows))
            continue;
        m_prefix.assign((const char*)data, pos);
        m_columns = cols;
        m_keyCols = c.keys;
        m_strCols = c.strs;
        m_rows.swap(rows);
        m_split = c.split;
        m_loaded = true;
        return true;
    }
    m_error = L"Unknown DB text table layout: no split of the columns into integer keys and strings matches the data.";
    return false;
}

void DBTextTable::read(iStream& in, DWORD fmt)
{
    aStream& s = in;
    const BYTE* data = s.m_data ? s.m_data + s.m_pos : 0;
    int size = s.m_size - s.m_pos;
    if (!parse(data, size))
        throw (const wchar_t*)m_error.c_str();
    s.m_pos = s.m_size;
}

std::string DBTextTable::serialize() const
{
    std::string out;
    size_t est = m_prefix.size() + 4;
    for (const Row& r : m_rows)
    {
        est += 8 * r.keys.size() + 4 * r.strs.size();
        for (const std::string& s : r.strs)
            est += s.size();
    }
    out.reserve(est);
    out = m_prefix;
    putDword(out, (unsigned)m_rows.size());
    for (const Row& r : m_rows)
    {
        for (long long k : r.keys)
            putInt64(out, k);
        for (const std::string& s : r.strs)
        {
            putDword(out, (unsigned)s.size());
            out += s;
        }
    }
    return out;
}

void DBTextTable::write(oStream& out, DWORD fmt) const
{
    if (!m_loaded)
        return;
    std::string bytes = serialize();
    out.put(bytes.data(), (long)bytes.size());
}

std::wstring DBTextTable::fieldName(int field) const
{
    if (field < 0 || field >= fieldCount())
        return std::wstring();
    int col = field < keyCount() ? m_keyCols[field] : m_strCols[field - keyCount()];
    return col < 0 ? std::wstring(L"(key)") : m_columns[col];
}

const wchar_t* DBTextTable::splitName() const
{
    switch (m_split)
    {
    case kByName:       return L"by name";
    case kImplicitKey:  return L"implicit key";
    case kLeadingKeys:  return L"leading keys";
    default:            return L"none";
    }
}

std::wstring DBTextTable::statusText() const
{
    wchar_t text[160];
    swprintf_s(text, L"%d rows, %d columns (%d integer keys, split %s), code page %d",
               rowCount(), fieldCount(), keyCount(), splitName(), getCodePage());
    return text;
}

std::wstring DBTextTable::notEncodableText() const
{
    wchar_t msg[200];
    swprintf_s(msg, L"The text contains characters that code page %d cannot store. The cell was not changed.",
               getCodePage());
    return msg;
}

std::wstring DBTextTable::decode(const std::string& bytes) const
{
    if (bytes.empty())
        return std::wstring();
    int n = MultiByteToWideChar(m_codePage, 0, bytes.data(), (int)bytes.size(), 0, 0);
    if (n <= 0)
    {
        std::wstring w;
        for (unsigned char c : bytes)
            w += (wchar_t)c;
        return w;
    }
    std::wstring w(n, L'\0');
    MultiByteToWideChar(m_codePage, 0, bytes.data(), (int)bytes.size(), &w[0], n);
    return w;
}

bool DBTextTable::encode(const wchar_t* text, std::string& out) const
{
    out.clear();
    int len = text ? (int)wcslen(text) : 0;
    if (!len)
        return true;
    BOOL usedDefault = FALSE;
    BOOL* pUsed = (m_codePage == CP_UTF8 || m_codePage == CP_UTF7) ? 0 : &usedDefault;
    DWORD flags = pUsed ? WC_NO_BEST_FIT_CHARS : 0;
    int n = WideCharToMultiByte(m_codePage, flags, text, len, 0, 0, 0, pUsed);
    if (n <= 0)
        return false;
    out.resize(n);
    WideCharToMultiByte(m_codePage, flags, text, len, &out[0], n, 0, pUsed);
    return !usedDefault;
}

std::wstring DBTextTable::getText(int row, int field) const
{
    if (row < 0 || row >= rowCount() || field < 0 || field >= fieldCount())
        return std::wstring();
    const Row& r = m_rows[row];
    if (field < keyCount())
    {
        wchar_t buf[32];
        _i64tow_s(r.keys[field], buf, 32, 10);
        return buf;
    }
    return decode(r.strs[field - keyCount()]);
}

long long DBTextTable::getKey(int row, int field) const
{
    if (row < 0 || row >= rowCount() || !isKeyField(field))
        return 0;
    return m_rows[row].keys[field];
}

int DBTextTable::setText(int row, int field, const wchar_t* text)
{
    if (row < 0 || row >= rowCount() || field < 0 || field >= fieldCount())
        return kBadCell;
    Row& r = m_rows[row];
    if (field < keyCount())
    {
        if (!text)
            return kBadNumber;
        while (*text == L' ')
            ++text;
        wchar_t* end = 0;
        errno = 0;
        long long v = _wcstoi64(text, &end, 10);
        while (end && *end == L' ')
            ++end;
        if (end == text || !end || *end || errno == ERANGE)
            return kBadNumber;
        if (r.keys[field] == v)
            return kUnchanged;
        r.keys[field] = v;
        return kChanged;
    }
    std::string bytes;
    if (!encode(text, bytes))
        return kNotEncodable;
    std::string& cell = r.strs[field - keyCount()];
    if (decode(cell) == (text ? text : L""))
        return kUnchanged;
    cell.swap(bytes);
    return kChanged;
}

int DBTextTable::insertRow(int index)
{
    if (!m_loaded)
        return -1;
    if (index < 0 || index > rowCount())
        index = rowCount();
    Row row;
    row.keys.assign(m_keyCols.size(), 0);
    row.strs.assign(m_strCols.size(), std::string());
    if (index > 0 && !row.keys.empty())
    {
        row.keys = m_rows[index - 1].keys;
        ++row.keys.back();
    }
    m_rows.insert(m_rows.begin() + index, std::move(row));
    return index;
}

bool DBTextTable::deleteRow(int index)
{
    if (index < 0 || index >= rowCount())
        return false;
    m_rows.erase(m_rows.begin() + index);
    return true;
}
