#include "String.hpp"
#include "iStream.hpp"
#include "oStream.hpp"
#include "Exception.h"
#include "primitiveVector.h"
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <shlwapi.h>

wchar_t g_stringEol[3] = L"\r\n";

static int initLegacyPrintf()
{
    _CRT_INTERNAL_LOCAL_PRINTF_OPTIONS &= ~(unsigned __int64)_CRT_INTERNAL_PRINTF_STANDARD_ROUNDING;
    _CRT_INTERNAL_LOCAL_PRINTF_OPTIONS |= _CRT_INTERNAL_PRINTF_LEGACY_THREE_DIGIT_EXPONENTS
                                        | _CRT_INTERNAL_PRINTF_LEGACY_MSVCRT_COMPATIBILITY;
    return 0;
}
static int g_legacyPrintf = initLegacyPrintf();

static inline float absf(float v) { return fabsf(v); }

String::String()
    : m_buffer(0), m_length(0), m_capacity(0), m_indent(0), m_radix(10), m_thousandSep(0), m_flags(0)
{
}

String::String(const wchar_t* text, unsigned length)
    : m_buffer(0), m_length(0), m_capacity(0), m_indent(0), m_radix(10), m_thousandSep(0), m_flags(0)
{
    append(text, length);
}

String::String(const String& other)
    : m_buffer(0), m_length(0), m_capacity(0), m_indent(0), m_radix(10), m_thousandSep(0), m_flags(0)
{
    m_flags = other.m_flags;
    append(other.m_buffer, other.m_length);
}

String::~String()
{
    if (m_buffer)
    {
        ::operator delete[](m_buffer);
        m_buffer = 0;
    }
}

String& String::operator=(const String& other)
{
    if (&other != this)
    {
        const wchar_t* text = other.m_buffer;
        unsigned length = other.m_length;
        clear();
        append(text, length);
    }
    return *this;
}

String& String::operator=(const wchar_t* text)
{
    if (text == m_buffer)
        return *this;
    clear();
    if (!text)
        return *this;
    unsigned length = (unsigned)wcslen(text);
    if (length && reserve(length + m_length))
    {
        memcpy(m_buffer + m_length, text, 2 * length);
        m_length += length;
        m_buffer[m_length] = 0;
    }
    return *this;
}

String& String::assign(const char* text, int length, UINT codePage)
{
    clear();
    return append(text, length, codePage);
}

String& String::assign(const wchar_t* text, unsigned length)
{
    clear();
    append(text, length);
    return *this;
}

String& String::assign(HGLOBAL hMem)
{
    clear();
    if (hMem)
    {
        int size = (int)GlobalSize(hMem);
        void* data = GlobalLock(hMem);
        append((const wchar_t*)data, size / 2);
        GlobalUnlock(hMem);
    }
    return *this;
}

bool String::reserve(unsigned length)
{
    if (m_capacity >= length + 1)
        return true;
    unsigned want = ((length + 1) & 0x7FFFFF00) + 256;
    unsigned capacity;
    if (want < 0x104)            capacity = 260;
    else if (want < 0x400)       capacity = 1024;
    else if (want < 0x4000)      capacity = 0x4000;
    else if (want < 0x10000)     capacity = 0x10000;
    else if (want < 0x40000)     capacity = 0x40000;
    else if (want < 0x100000)    capacity = 0x100000;
    else if (want < 0x800000)    capacity = 0x800000;
    else if (want < 0x2000000)   capacity = 0x2000000;
    else if (want < 0x8000000)   capacity = 0x8000000;
    else if (want < 0x40000000)  capacity = 0x40000000;
    else if (want < 0x77359400)  capacity = 2000000000;
    else                         return false;

    wchar_t* buffer = (wchar_t*)::operator new[](((unsigned __int64)capacity >> 31) != 0 ? (size_t)-1 : 2 * capacity);
    if (!buffer)
        return false;
    memset(buffer, 0, 2 * capacity);
    if (m_length)
    {
        memcpy(buffer, m_buffer, 2 * m_length);
        ::operator delete[](m_buffer);
    }
    m_buffer = buffer;
    m_capacity = capacity;
    return true;
}

String& String::append(const char* text)
{
    if (text)
    {
        int count = (int)strlen(text);
        if (reserve(count + m_length))
        {
            for (int i = 0; i < count; ++i)
                m_buffer[i + m_length] = (wchar_t)(short)text[i];
            m_length += count;
        }
    }
    return *this;
}

String& String::append(const char* text, int length, UINT codePage)
{
    if (codePage == 1200)
        return append((const wchar_t*)text, (unsigned)length);
    int count = MultiByteToWideChar(codePage, 0, text, length, 0, 0);
    if (count > 0 && reserve(count + m_length))
    {
        MultiByteToWideChar(codePage, 0, text, length, m_buffer + m_length, m_capacity - m_length);
        m_length += count;
        m_buffer[m_length] = 0;
    }
    return *this;
}

String& String::append(const wchar_t* text, unsigned length)
{
    if (length == (unsigned)-1)
    {
        if (!text)
            return *this;
        length = (unsigned)wcslen(text);
    }
    if (text && length && reserve(length + m_length))
    {
        memcpy(m_buffer + m_length, text, 2 * length);
        m_length += length;
        m_buffer[m_length] = 0;
    }
    return *this;
}

void String::clear()
{
    m_length = 0;
    m_capacity = 0;
    if (m_buffer)
    {
        ::operator delete[](m_buffer);
        m_buffer = 0;
    }
}

bool String::equalsNoCase(const wchar_t* text) const
{
    if (!text || !*text)
        return m_length == 0;
    return wcslen(text) == m_length && _wcsnicmp(m_buffer, text, m_length) == 0;
}

bool String::operator==(const wchar_t* text) const
{
    if (!text || !*text)
        return m_length == 0;
    return wcslen(text) == m_length && _wcsnicmp(m_buffer, text, m_length) == 0;
}

int String::compare(const String& other) const
{
    unsigned otherLength = other.m_length;
    unsigned common = m_length < otherLength ? m_length : otherLength;
    for (unsigned i = 0; i < common; ++i)
    {
        unsigned short a = m_buffer[i];
        unsigned short b = other.m_buffer[i];
        if (a < b)
            return -1;
        if (a > b)
            return 1;
    }
    if (m_length == otherLength)
        return 0;
    return m_length < otherLength ? -1 : 1;
}

bool String::containsNoCase(const wchar_t* text) const
{
    if (!text || wcslen(text) > m_length)
        return false;
    String needle(text, (unsigned)-1);
    String haystack(*this);
    needle.toLowerInPlace();
    haystack.toLowerInPlace();
    return wcsstr(haystack.m_buffer, needle.m_buffer) != 0;
}

bool String::equalsLowerCase(const wchar_t* text, unsigned length) const
{
    if (!text || length != m_length)
        return false;
    String lower(*this);
    lower.toLowerInPlace();
    return memcmp(lower.m_buffer, text, 2 * m_length) == 0;
}

bool String::startsWith(const wchar_t* text) const
{
    if (!text)
        return false;
    size_t n = wcslen(text);
    return n <= m_length && wcsncmp(m_buffer, text, n) == 0;
}

bool String::endsWithNoCase(const wchar_t* text) const
{
    if (!text)
        return false;
    size_t n = wcslen(text);
    return n <= m_length && _wcsnicmp(m_buffer + (m_length - n), text, n) == 0;
}

int String::indexOf(const wchar_t* text) const
{
    if (m_buffer && text)
    {
        const wchar_t* found = wcsstr(m_buffer, text);
        if (found)
            return (int)(found - m_buffer);
    }
    return -1;
}

int String::indexOf(wchar_t c) const
{
    if (!m_length)
        return -1;
    for (unsigned i = 0; i < m_length; ++i)
        if (m_buffer[i] == c)
            return (int)i;
    return -1;
}

int String::lastIndexOf(wchar_t c) const
{
    if (!m_length)
        return -1;
    int index = (int)m_length - 1;
    if (!index)
        return -1;
    for (const wchar_t* p = m_buffer + index; *p != c; --p)
    {
        if (!--index)
            return -1;
    }
    return index;
}

bool String::isHex() const
{
    if (!m_length)
        return false;
    for (unsigned i = 0; i < m_length; ++i)
    {
        unsigned c = m_buffer[i];
        if ((c < 'A' || c > 'F') && (c < 'a' || c > 'f') && (c < '0' || c > '9'))
            return false;
    }
    return true;
}

int String::toInt() const
{
    if (m_length)
    {
        unsigned i = 0;
        for (; i < m_length; ++i)
        {
            unsigned c = m_buffer[i];
            if ((c < '0' || c > '9') && c != '-')
                break;
        }
        if (i >= m_length)
            return _wtoi(m_buffer);
    }
    return (int)0x80000000;
}

unsigned String::toHex() const
{
    if (!isHex())
        return 0x80000000;
    unsigned result = 0;
    for (unsigned i = 0; i < m_length; ++i)
    {
        unsigned c = m_buffer[i];
        if ((unsigned short)(c - '0') <= 9u)
            result = c + 16 * (result - 3);
        if ((unsigned short)(c - 'A') <= 5u)
            result = c + 16 * result - 55;
        if ((unsigned short)(c - 'a') <= 5u)
            result = c + 16 * result - 87;
    }
    return result;
}

int String::compareLogical(const String& a, const String& b)
{
    if (!a.m_buffer)
        return -(b.m_buffer != 0);
    if (b.m_buffer)
        return StrCmpLogicalW(a.m_buffer, b.m_buffer);
    return 1;
}

void String::toLowerInPlace()
{
    for (unsigned i = 0; i < m_length; ++i)
    {
        wchar_t c = m_buffer[i];
        if ((unsigned short)(c - 'A') <= 0x19u)
            m_buffer[i] = c + 32;
    }
}

String& String::toLower()
{
    for (unsigned i = 0; i < m_length; ++i)
    {
        wchar_t c = m_buffer[i];
        if ((unsigned short)(c - 'A') <= 0x19u)
            m_buffer[i] = c + 32;
    }
    return *this;
}

HGLOBAL String::toHGlobal() const
{
    size_t bytes = 2 * m_length;
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, bytes + 2);
    void* data = GlobalLock(hMem);
    memcpy(data, m_buffer, bytes);
    GlobalUnlock(hMem);
    return hMem;
}

String String::trimLeft(wchar_t c) const
{
    if (!m_length)
        return String(*this);
    unsigned count = m_length;
    const wchar_t* p = m_buffer;
    while (count && *p == c)
    {
        ++p;
        --count;
    }
    return String(p, count);
}

String String::trimRight(wchar_t ) const
{
    unsigned count = m_length;
    if (!count)
        return String(*this);
    const wchar_t* p = m_buffer + count - 1;
    while (count && *p == L')')
    {
        --p;
        --count;
    }
    return String(m_buffer, count);
}

String& String::stripRight(wchar_t c, int )
{
    int remaining = -1;
    if (m_length && m_buffer)
    {
        wchar_t* p = m_buffer + m_length - 1;
        do
        {
            if (*p != c)
                break;
            if (!remaining)
                break;
            *p = 0;
            --m_length;
            --p;
            --remaining;
        } while (m_length);
    }
    return *this;
}

void String::stripCrLf()
{
    if (m_length && m_buffer && m_length >= 2)
    {
        do
        {
            if (m_buffer[m_length - 2] != 13)
                break;
            if (m_buffer[m_length - 1] != 10)
                break;
            m_buffer[m_length - 1] = 0;
            m_buffer[m_length - 2] = 0;
            m_length -= 2;
        } while (m_length >= 2);
    }
}

String& String::removeExtension()
{
    unsigned length = m_length;
    if (length)
    {
        unsigned index = length - 1;
        if (length != 1)
        {
            for (wchar_t* p = m_buffer + index; *p != L'.'; --p)
            {
                if (!--index)
                    return *this;
            }
            if (index != (unsigned)-1 && index + 16 > length && index < length)
            {
                memset(m_buffer + index, 0, 2 * (length - index));
                m_length = index;
            }
        }
    }
    return *this;
}

void String::removeFileName()
{
    unsigned length = m_length;
    if (length)
    {
        unsigned index = length - 1;
        if (length != 1)
        {
            for (wchar_t* p = m_buffer + index; *p != L'\\'; --p)
            {
                if (!--index)
                    return;
            }
            if (index != (unsigned)-1)
            {
                unsigned keep = index + 1;
                if (keep < length)
                {
                    memset(m_buffer + keep, 0, 2 * (length - keep));
                    m_length = keep;
                }
            }
        }
    }
}

bool String::toParentPath()
{
    if (m_length)
    {
        wchar_t last = m_buffer[m_length - 1];
        if (last == L'\\' || last == L'/')
        {
            --m_length;
            m_buffer[m_length] = 0;
        }
    }
    unsigned length = m_length;
    if (!length)
        return false;
    unsigned index = length - 1;
    bool found = false;
    if (length != 1)
    {
        const wchar_t* p = m_buffer + index;
        found = true;
        while (*p != L'\\')
        {
            --p;
            if (!--index)
            {
                found = false;
                break;
            }
        }
    }
    if (!found)
    {
        index = length - 1;
        if (length == 1)
            return false;
        for (const wchar_t* p = m_buffer + index; *p != L'/'; --p)
        {
            if (!--index)
                return false;
        }
        if (index == (unsigned)-1)
            return false;
    }
    unsigned keep = index + 1;
    if (keep < length)
    {
        memset(m_buffer + keep, 0, 2 * (length - keep));
        m_length = keep;
    }
    return true;
}

String String::getExtension() const
{
    const wchar_t* ext = PathFindExtensionW(m_buffer);
    String result(ext, (unsigned)-1);
    result.toLower();
    return result;
}

String String::hashToFormat() const
{
    String result;
    for (unsigned i = 0; i < m_length; ++i)
    {
        if (m_buffer[i] == L'#')
            result << L"%s";
        else
            result << m_buffer[i];
    }
    return result;
}

String String::unescape() const
{
    String result;
    for (unsigned i = 0; i < m_length; ++i)
    {
        wchar_t c = m_buffer[i];
        if (c != L'\\')
        {
            result << c;
            continue;
        }
        wchar_t next = m_buffer[++i];
        if (next == L'n' || next == L'r')
            result << L"\r\n";
        else if (next == L't')
            result << L'\t';
        else
        {
            result << L'\\';
            result << next;
        }
    }
    return result;
}

String String::escape() const
{
    String result;
    for (unsigned i = 0; i < m_length; ++i)
    {
        wchar_t c = m_buffer[i];
        if (c != 13)
        {
            result << c;
            continue;
        }
        wchar_t next = m_buffer[++i];
        if (next == 10)
        {
            result << L'\\';
            result << L'n';
        }
        else
        {
            result << (wchar_t)13;
            result << next;
        }
    }
    return result;
}

String String::lfToCrLf() const
{
    String result;
    for (unsigned i = 0; i < m_length; ++i)
    {
        if (m_buffer[i] == 10)
        {
            result << (wchar_t)13;
            result << (wchar_t)10;
        }
        else
            result << m_buffer[i];
    }
    return result;
}

String String::crLfToLf() const
{
    String result;
    for (unsigned i = 0; i < m_length; ++i)
    {
        if (m_buffer[i] != 13)
        {
            result << m_buffer[i];
            continue;
        }
        ++i;
        if (m_buffer[i] == 10)
            result << (wchar_t)10;
        else
            result << (wchar_t)13;
    }
    return result;
}

String String::mid(unsigned start) const
{
    return substring(start, m_length - start);
}

String String::left(unsigned count) const
{
    if (count <= m_length)
        return substring(0, count);
    return String(*this);
}

String String::right(unsigned count) const
{
    if (count <= m_length)
        return substring(m_length - count, count);
    return String(*this);
}

String String::substring(unsigned start, unsigned count) const
{
    String part;
    if (start + count > m_length)
    {
        count = m_length - start;
        if (m_length == start)
            return String(part);
    }
    part.append(m_buffer + start, count);
    return String(part);
}

String String::sqlQuote(const wchar_t* text)
{
    if (!text || !*text)
        return String(L"null", (unsigned)-1);
    int count = (int)wcslen(text);
    String quoted(L"'", (unsigned)-1);
    for (; count > 0; --count, ++text)
    {
        if (*text == L'\'')
            quoted << L"''";
        else
            quoted << *text;
    }
    quoted << L"'";
    return String(quoted);
}

int String::writeTo(FILE* file, UINT codePage) const
{
    if (!file)
        return 161;
    size_t written = 0;
    if (m_length)
    {
        if (codePage == 1200)
        {
            size_t bom = 0xFEFF;
            fwrite(&bom, 2, 1, file);
            written = fwrite(m_buffer, 2 * m_length, 1, file);
        }
        else
        {
            char* bytes = (char*)::operator new[](2 * m_length);
            BOOL usedDefault = FALSE;
            int count = WideCharToMultiByte(codePage, 0, m_buffer, m_length, bytes, 2 * m_length,
                                            (LPCCH)L"?", &usedDefault);
            size_t result = fwrite(bytes, count, 1, file);
            ::operator delete[](bytes);
            written = result;
        }
    }
    fflush(file);
    fclose(file);
    if (written == 1)
        return 0;
    return m_length != 0 ? 29 : 232;
}

int String::saveAs(const wchar_t* fileName, int , UINT codePage) const
{
    FILE* file = 0;
    _wfopen_s(&file, fileName, L"wb");
    return writeTo(file, codePage);
}

String& String::newline()
{
    *this << g_stringEol;
    for (int i = m_indent; i > 0; --i)
        *this << L'\t';
    return *this;
}

String& String::operator<<(unsigned char v)
{
    wchar_t buffer[8] = { 0 };
    _itow_s(v, buffer, 8, m_radix);
    size_t length = wcslen(buffer);
    if (m_radix == 16)
    {
        for (size_t i = 0; i < length; ++i)
        {
            wchar_t c = buffer[i];
            if ((unsigned short)(c - 'a') <= 0x19u)
                buffer[i] = c - 32;
        }
        if (length == 1)
        {
            buffer[1] = buffer[0];
            buffer[2] = 0;
            buffer[0] = L'0';
        }
    }
    else if (m_flags & SF_PAD_SPACES)
        swprintf_s(buffer, 8, L"% 3u", (unsigned)v);
    else if (m_flags & SF_PAD_ZEROS)
        swprintf_s(buffer, 8, L"%03u", (unsigned)v);
    return append(buffer, (unsigned)wcslen(buffer));
}

String& String::operator<<(short v)
{
    wchar_t buffer[32];
    if (m_flags & SF_PAD_SPACES)
        swprintf_s(buffer, 32, L"% 4hi", v);
    else if (m_flags & SF_PAD_ZEROS)
        swprintf_s(buffer, 32, L"%04hi", v);
    else
        _itow_s(v, buffer, 32, m_radix);
    return append(buffer, (unsigned)wcslen(buffer));
}

String& String::operator<<(unsigned short v)
{
    wchar_t buffer[32];
    if (m_flags & SF_PAD_SPACES)
        swprintf_s(buffer, 32, L"% 5hu", v);
    else if (m_flags & SF_PAD_ZEROS)
        swprintf_s(buffer, 32, L"%05hu", v);
    else if (m_flags & SF_USHORT_SIGNED)
        swprintf_s(buffer, 32, L"%hi", v);
    else
        _itow_s(v, buffer, 32, m_radix);
    return append(buffer, (unsigned)wcslen(buffer));
}

String& String::operator<<(unsigned long v)
{
    wchar_t buffer[32];
    memset(buffer, 0, sizeof(buffer));
    _ltow_s((long)v, buffer, 32, m_radix);
    size_t length = wcslen(buffer);
    if (m_radix == 16)
    {
        for (size_t i = 0; i < length; ++i)
        {
            wchar_t c = buffer[i];
            if ((unsigned short)(c - 'a') <= 0x19u)
                buffer[i] = c - 32;
        }
    }
    return append(buffer, (unsigned)length);
}

String& String::operator<<(long v)
{
    wchar_t buffer[128];
    memset(buffer, 0, sizeof(buffer));
    const wchar_t* text = buffer;
    int radix = m_radix;
    _ltow_s(v, buffer, 64, radix);
    if (radix == 16)
    {
        *this << L'0';
        *this << L'x';
        size_t length = wcslen(buffer);
        if (length < 8)
        {
            for (size_t n = 8 - length; n; --n)
                *this << L'0';
        }
    }
    else
    {
        wchar_t separator = m_thousandSep;
        if (separator == L'=')
            swprintf_s(buffer, 64, L"% 10li", v);
        else if ((m_flags & 0x11) == 0x11)
            swprintf_s(buffer, 64, L"% 6li", v);
        else if (m_flags & SF_PAD_SPACES)
            swprintf_s(buffer, 64, L"% 8li", v);
        else if (separator)
        {
            unsigned short group = (unsigned short)((radix == 10) + 2);
            const wchar_t* first = &buffer[buffer[0] == L'-'];
            const wchar_t* src = wcschr(buffer, 0);
            wchar_t* out = &buffer[127];
            unsigned short inGroup = 0;
            while (src != first)
            {
                if (inGroup == group)
                {
                    *--out = separator;
                    inGroup = 0;
                }
                *--out = *--src;
                ++inGroup;
            }
            if (buffer[0] == L'-')
                *--out = L'-';
            text = out;
        }
    }
    unsigned length = (unsigned)wcslen(text);
    if (text && length && reserve(length + m_length))
    {
        memcpy(m_buffer + m_length, text, 2 * length);
        m_length += length;
        m_buffer[m_length] = 0;
    }
    return *this;
}

String& String::operator<<(int v)
{
    wchar_t buffer[64];
    memset(buffer, 0, sizeof(buffer));
    _itow_s(v, buffer, 64, m_radix);
    size_t length = wcslen(buffer);
    if (m_radix == 16)
    {
        for (size_t i = 0; i < length; ++i)
        {
            wchar_t c = buffer[i];
            if ((unsigned short)(c - 'a') <= 0x19u)
                buffer[i] = c - 32;
        }
    }
    return append(buffer, (unsigned)length);
}

String& String::operator<<(__int64 v)
{
    wchar_t buffer[162];
    _i64tow_s(v, buffer, 160, m_radix);
    size_t length = wcslen(buffer);
    if (m_radix == 16)
    {
        for (size_t i = 0; i < length; ++i)
        {
            wchar_t c = buffer[i];
            if ((unsigned short)(c - 'a') <= 0x19u)
                buffer[i] = c - 32;
        }
    }
    return append(buffer, (unsigned)length);
}

String& String::operator<<(float v)
{
    wchar_t buffer[66];
    if (v != 0.0f)
    {
        float magnitude = absf(v);
        if ((0.00001 > (double)magnitude || magnitude > 1.0e10f) && !(m_flags & SF_PAD_SPACES))
        {
            swprintf_s(buffer, 64, L"%+6.3e", (double)v);
            return append(buffer, (unsigned)wcslen(buffer));
        }
    }
    const wchar_t* format = L"%+8.6f";
    float value = v;
    if (m_flags & SF_FLOAT_WIDE)
        format = L"%+10.6f";
    else if (m_flags & SF_FIXED2)
    {
        if (m_flags & SF_PAD_SPACES)
        {
            format = L"% 9.2f";
            if (absf(v) < 1000.0f)
                format = L"% 9.3f";
        }
        else
            format = L"%+6.2f";
    }
    else if ((double)absf(v) < 0.00001)
        value = 0.0f;
    swprintf_s(buffer, 64, format, (double)value);
    return append(buffer, (unsigned)wcslen(buffer));
}

String& String::operator<<(const primitiveVector<long>& v)
{
    int last = v.m_count - 1;
    for (int i = 0; i < last; ++i)
    {
        long item = (i < 0 || i >= v.m_count) ? 0 : v.m_items[i];
        *this << item;
        *this << L',';
    }
    long item = (last < 0 || last >= v.m_count) ? 0 : v.m_items[last];
    *this << item;
    return *this;
}

void String::read(iStream& in, int lengthSize, UINT codePage)
{
    if (codePage == (UINT)-1)
        codePage = in.m_codePage;
    clear();
    unsigned length;
    if (lengthSize == 1)
        length = (unsigned char)in.readByte();
    else
    {
        if (lengthSize != 4)
        {
            Exception e;
            e.m_message = _wcsdup(L"Invalid size of string's length");
            throw e;
        }
        length = in.readInt();
    }
    if (length > 0x4000)
    {
        Exception e;
        e.m_message = _wcsdup(L"Invalid stream format");
        throw e;
    }
    if (!length)
        return;
    if (codePage == 1200)
    {
        if (in.m_size - in.m_pos < (int)(2 * length))
            throw ArrayIndexOutOfBoundsException(in.m_size - in.m_pos, 2 * length);
        append((const wchar_t*)(in.m_data + in.m_pos), length);
        in.skip(2 * length);
    }
    else
    {
        char* bytes = (char*)::operator new[](length);
        in.read(bytes, length);
        while (!bytes[length - 1])
        {
            if ((int)--length <= 0)
            {
                ::operator delete[](bytes);
                return;
            }
        }
        append(bytes, length, codePage);
        ::operator delete[](bytes);
    }
}

static inline unsigned long swap32(unsigned long v)
{
    return ((v << 8 | v >> 24) & 0xFF00FF) | ((v >> 8 | v << 24) & 0xFF00FF00);
}

void String::write(oStream& out, int lengthSize, UINT codePage) const
{
    if (codePage == (UINT)-1)
        codePage = out.m_codePage;
    unsigned count = m_length;
    if (count)
    {
        if (out.m_writeNul)
            ++count;
        int bytes = 2 * count;
        if (codePage == 1200)
        {
            unsigned long length = count;
            if (out.m_swapBytes)
                length = swap32(length);
            out.put(&length, lengthSize);
            out.append(m_buffer, bytes);
        }
        else
        {
            char* text = (char*)::operator new[](2 * count);
            BOOL usedDefault = FALSE;
            int written = WideCharToMultiByte(codePage, 0, m_buffer, m_length, text, bytes,
                                              (LPCCH)L"?", &usedDefault);
            if (out.m_writeNul)
                text[written++] = 0;
            unsigned long length = written;
            if (out.m_swapBytes)
                length = swap32(length);
            out.put(&length, lengthSize);
            out.append(text, written);
            ::operator delete[](text);
        }
    }
    else if (out.m_writeNul)
    {
        unsigned long one = 1;
        if (out.m_swapBytes)
            one = swap32(one);
        out.put(&one, lengthSize);
        unsigned long zero = 0;
        if (out.m_swapBytes)
            zero = swap32(zero);
        out.put(&zero, (codePage == 1200) + 1);
    }
    else
    {
        unsigned long zero = 0;
        if (out.m_swapBytes)
            zero = swap32(zero);
        out.put(&zero, lengthSize);
    }
}

String operator+(const String& a, const String& b)
{
    String temp(a);
    temp.append(b.m_buffer, b.m_length);
    return String(temp);
}

String operator+(const String& a, const wchar_t* b)
{
    String temp(a);
    temp << b;
    return String(temp);
}
