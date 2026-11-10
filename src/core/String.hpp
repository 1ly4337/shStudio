#pragma once
#include <windows.h>
#include <cstdio>
#include <cwchar>

class iStream;
class oStream;
template <class T> class primitiveVector;

enum StringFormatFlags
{
    SF_PAD_SPACES  = 0x01,
    SF_USHORT_SIGNED = 0x02,
    SF_FLOAT_WIDE  = 0x04,
    SF_PAD_ZEROS   = 0x08,
    SF_FIXED2      = 0x10,
};

extern wchar_t g_stringEol[3];

class String
{
public:
    String();
    String(const wchar_t* text, unsigned length = (unsigned)-1);
    String(const String& other);
    virtual ~String();

    String& operator=(const String& other);
    String& operator=(const wchar_t* text);
    String& assign(const char* text, int length, UINT codePage);
    String& assign(const wchar_t* text, unsigned length);
    String& assign(HGLOBAL hMem);

    bool reserve(unsigned length);
    String& append(const char* text);
    String& append(const char* text, int length, UINT codePage);
    String& append(const wchar_t* text, unsigned length = (unsigned)-1);
    void clear();

    String& operator<<(char c)    { wchar_t w = (wchar_t)(short)c; return put(w); }
    String& operator<<(wchar_t c) { return put(c); }
    String& operator<<(const wchar_t* text)
    {
        if (text == L"\r\n")
            return newline();
        if (text)
            append(text, (unsigned)wcslen(text));
        return *this;
    }
    String& operator<<(const char* text)
    {
        if (text == "\r\n")
            return newline();
        return append(text);
    }
    String& operator<<(const String& s) { return append(s.m_buffer, s.m_length); }
    String& operator<<(unsigned char v);
    String& operator<<(short v);
    String& operator<<(unsigned short v);
    String& operator<<(unsigned long v);
    String& operator<<(long v);
    String& operator<<(int v);
    String& operator<<(__int64 v);
    String& operator<<(float v);
    String& operator<<(const primitiveVector<long>& v);
    String& newline();

    bool equalsNoCase(const wchar_t* text) const;
    bool operator==(const wchar_t* text) const;
    bool sub_4121F0(const wchar_t* text) const;
    int  compare(const String& other) const;
    bool containsNoCase(const wchar_t* text) const;
    bool equalsLowerCase(const wchar_t* text, unsigned length) const;
    bool startsWith(const wchar_t* text) const;
    bool endsWithNoCase(const wchar_t* text) const;
    bool equalsNoCase(const String& other) const;
    bool startsWith(const String& prefix) const;
    bool differsNoCase(const String& other) const;
    int  indexOf(const wchar_t* text) const;
    int  indexOf(wchar_t c) const;
    int  lastIndexOf(wchar_t c) const;
    bool isHex() const;
    int  toInt() const;
    unsigned toHex() const;
    static int compareLogical(const String& a, const String& b);

    void toLowerInPlace();
    void toUpperAscii();
    bool endsWith(wchar_t c) const { return m_length && m_buffer[m_length - 1] == c; }
    String& toLower();
    HGLOBAL toHGlobal() const;
    String trimLeft(wchar_t c) const;
    String trimRight(wchar_t c) const;
    String& stripRight(wchar_t c, int maxCount = -1);
    void stripCrLf();
    String& removeExtension();
    void removeFileName();
    bool toParentPath();
    String getExtension() const;
    String hashToFormat() const;
    String unescape() const;
    String escape() const;
    String lfToCrLf() const;
    String crLfToLf() const;
    String mid(unsigned start) const;
    String left(unsigned count) const;
    String right(unsigned count) const;
    String substring(unsigned start, unsigned count) const;
    static String sqlQuote(const wchar_t* text);

    int writeTo(FILE* file, UINT codePage) const;
    int saveAs(const wchar_t* fileName, int unused, UINT codePage) const;
    void read(iStream& in, int lengthSize, UINT codePage);
    void write(oStream& out, int lengthSize, UINT codePage) const;

    const wchar_t* c_str() const { return m_buffer; }
    unsigned length() const { return m_length; }

private:
    String& put(wchar_t c)
    {
        if (reserve(m_length + 1))
        {
            m_buffer[m_length++] = c;
            m_buffer[m_length] = 0;
        }
        return *this;
    }

public:
    wchar_t* m_buffer;
    unsigned m_length;
    unsigned m_capacity;
    int      m_indent;
    int      m_radix;
    wchar_t  m_thousandSep;
    unsigned m_flags;
};
static_assert(sizeof(String) == 0x20, "String size");

String operator+(const String& a, const String& b);
String operator+(const String& a, const wchar_t* b);
