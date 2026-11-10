#include "iStream.hpp"
#include "Exception.h"
#include <cstring>
#include <cstdlib>

static inline unsigned long swap32(unsigned long v)
{
    return ((v >> 8 | v << 24) & 0xFF00FF00) | ((v << 8 | v >> 24) & 0xFF00FF);
}

char iStream::readByte()
{
    int available = m_size - m_pos;
    if (available < 1)
        throw ArrayIndexOutOfBoundsException(available, 1);
    char value = (char)m_data[m_pos];
    int pos = m_pos + 1;
    if (pos > m_size)
        throw ArrayIndexOutOfBoundsException(pos, m_size);
    m_pos = pos;
    return value;
}

short iStream::readShort()
{
    int available = m_size - m_pos;
    if (available < 2)
        throw ArrayIndexOutOfBoundsException(available, 2);
    unsigned short value = *(unsigned short*)(m_data + m_pos);
    if (m_swapBytes)
        value = (unsigned short)((value >> 8) | (value << 8));
    int pos = m_pos + 2;
    if (pos > m_size)
        throw ArrayIndexOutOfBoundsException(pos, m_size);
    m_pos = pos;
    return (short)value;
}

unsigned long iStream::peekInt()
{
    int available = m_size - m_pos;
    if (available < 4)
        throw ArrayIndexOutOfBoundsException(available, 4);
    unsigned long value = *(unsigned long*)(m_data + m_pos);
    if (m_swapBytes)
        return swap32(value);
    return value;
}

unsigned long iStream::readInt()
{
    unsigned long value = peekInt();
    int pos = m_pos + 4;
    if (pos > m_size)
        throw ArrayIndexOutOfBoundsException(pos, m_size);
    m_pos = pos;
    return value;
}

unsigned long iStream::readDword()
{
    int available = m_size - m_pos;
    if (available < 4)
        throw ArrayIndexOutOfBoundsException(available, 4);
    unsigned long value = *(unsigned long*)(m_data + m_pos);
    if (m_swapBytes)
        value = swap32(value);
    int pos = m_pos + 4;
    if (pos > m_size)
        throw ArrayIndexOutOfBoundsException(pos, m_size);
    m_pos = pos;
    return value;
}

__int64 iStream::readInt64()
{
    int available = m_size - m_pos;
    if (available < 8)
        throw ArrayIndexOutOfBoundsException(available, 8);
    unsigned __int64 value = *(unsigned __int64*)(m_data + m_pos);
    if (m_swapBytes)
        value = _byteswap_uint64(value);
    int pos = m_pos + 8;
    if (pos > m_size)
        throw ArrayIndexOutOfBoundsException(pos, m_size);
    m_pos = pos;
    return (__int64)value;
}

float iStream::readFloat()
{
    int available = m_size - m_pos;
    if (available < 4)
        throw ArrayIndexOutOfBoundsException(available, 4);
    float value = *(float*)(m_data + m_pos);
    int pos = m_pos + 4;
    if (pos > m_size)
        throw ArrayIndexOutOfBoundsException(pos, m_size);
    m_pos = pos;
    return value;
}

void iStream::read(void* dst, int size)
{
    if (dst && size > 0)
    {
        int available = m_size - m_pos;
        if (available < size)
            throw ArrayIndexOutOfBoundsException(available, size);
        memcpy(dst, m_data + m_pos, size);
        m_pos += size;
    }
}

void* iStream::readAlloc(int size)
{
    int available = m_size - m_pos;
    if (available < size)
        throw ArrayIndexOutOfBoundsException(available, size);
    void* data = ::operator new[](size);
    memcpy(data, m_data + m_pos, size);
    m_pos += size;
    return data;
}

void iStream::read(aStream& dst, int size)
{
    int available = m_size - m_pos;
    if (available < size)
        throw ArrayIndexOutOfBoundsException(available, size);
    dst.wipe();
    dst.allocate(size);
    memcpy(dst.m_data + dst.m_pos, m_data + m_pos, size);
    m_pos += size;
    dst.m_size = size;
}

void iStream::gets(void* dst, int size)
{
    if (!dst || size <= 0)
        throw IllegalArgumentException(L"iStream::gets : Invalid argument(s)");
    memset(dst, 0, size);
    int available = m_size - m_pos;
    if (available < size)
        throw ArrayIndexOutOfBoundsException(available, size);
    memcpy(dst, m_data + m_pos, size);
    m_pos += size;
}
