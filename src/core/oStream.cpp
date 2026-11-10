#include "oStream.hpp"
#include "iStream.hpp"
#include "Exception.h"
#include <cstring>
#include <cstdlib>
#include <cwchar>

static inline unsigned long swap32(unsigned long v)
{
    return ((v << 8 | v >> 24) & 0xFF00FF) | ((v >> 8 | v << 24) & 0xFF00FF00);
}

oStream& oStream::put(const void* data, long size)
{
    if (size && !data)
        throw IllegalArgumentException(1, L"oStream::put(const void*, long)");
    if (size & 0xF0000000)
        throw IllegalArgumentException(2, L"oStream::put(const void*, long)");
    if (!append(data, size))
        throw OutOfMemoryException(size, L"oStream::put(byte, long)");
    return *this;
}

oStream& oStream::operator<<(const wchar_t* text)
{
    unsigned long length = text ? (unsigned long)wcslen(text) : 0;
    put(&length, 1);
    if (text)
        put(text, 2 * length);
    return *this;
}

oStream& oStream::operator<<(short v)
{
    unsigned long value = (unsigned short)v;
    if (m_swapBytes)
        value = (((unsigned short)v << 8) & 0xFF00) | (unsigned char)((unsigned short)v >> 8);
    if (!append(&value, 2))
        throw OutOfMemoryException(2, L"oStream::put(byte, long)");
    return *this;
}

oStream& oStream::operator<<(char v)
{
    char value[4];
    value[0] = v;
    if (!append(value, 1))
        throw OutOfMemoryException(1, L"oStream::put(byte, long)");
    return *this;
}

oStream& oStream::operator<<(unsigned short v)
{
    unsigned long value = v;
    if (m_swapBytes)
        value = (unsigned char)(v >> 8) | ((v << 8) & 0xFF00);
    if (!append(&value, 2))
        throw OutOfMemoryException(2, L"oStream::put(byte, long)");
    return *this;
}

oStream& oStream::operator<<(unsigned long v)
{
    unsigned long value = v;
    if (m_swapBytes)
        value = swap32(v);
    if (!append(&value, 4))
        throw OutOfMemoryException(4, L"oStream::put(byte, long)");
    return *this;
}

oStream& oStream::operator<<(__int64 v)
{
    unsigned __int64 value = (unsigned __int64)v;
    if (m_swapBytes)
        value = _byteswap_uint64(value);
    if (!append(&value, 8))
        throw OutOfMemoryException(8, L"oStream::put(byte, long)");
    return *this;
}

oStream& oStream::operator<<(float v)
{
    float value = v;
    if (!append(&value, 4))
        throw OutOfMemoryException(4, L"oStream::put(byte, long)");
    return *this;
}

int oStream::put(iStream& src, int size)
{
    int needed = size + m_size;
    if (m_capacity < needed && !grow(needed - m_capacity))
        throw L"mem Error";
    const void* data = src.m_data + src.m_pos;
    append(data, size);
    int pos = size + src.m_pos;
    if (pos > src.m_size)
        throw ArrayIndexOutOfBoundsException(pos, src.m_size);
    src.m_pos = pos;
    return pos;
}
