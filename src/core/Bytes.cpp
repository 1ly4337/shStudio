#include "Bytes.h"
#include <cstring>

Bytes::Bytes(int , UINT codePage) : aStream(false, codePage), iStream(), oStream()
{
    m_flag = false;
    m_writeNul = true;
}

Bytes::Bytes(const void* data, int size, int ) : aStream(false, 0), iStream(), oStream()
{
    m_flag = false;
    m_writeNul = true;
    if (data && size >= 1)
    {
        if (m_capacity >= size || grow(size - m_capacity))
        {
            memset(m_data, 0, m_capacity);
            memcpy(m_data, data, size);
            m_size = size;
            m_pos = 0;
        }
    }
}

Bytes::Bytes(const Bytes& other) : aStream(other.m_swapBytes, 0), iStream(), oStream()
{
    m_flag = false;
    m_writeNul = true;
    m_swapBytes = other.m_swapBytes;
    int size = other.m_size;
    const BYTE* data = other.m_data;
    if (data && size >= 1)
    {
        if (m_capacity >= size || grow(size - m_capacity))
        {
            memset(m_data, 0, m_capacity);
            memcpy(m_data, data, size);
            m_size = size;
            m_pos = 0;
        }
    }
}

Bytes::~Bytes()
{
}

int Bytes::load(const wchar_t* fileName)
{
    return aStream::load(fileName);
}

int Bytes::save(const wchar_t* fileName)
{
    return aStream::save(fileName);
}

Bytes& Bytes::operator=(const Bytes& other)
{
    int size = other.m_size;
    const BYTE* data = other.m_data;
    if (data && size >= 1)
    {
        if (m_capacity >= size || grow(size - m_capacity))
        {
            memset(m_data, 0, m_capacity);
            memcpy(m_data, data, size);
            m_size = size;
            m_pos = 0;
        }
    }
    return *this;
}

void Bytes::release()
{
    if (m_data)
        memset(m_data, 0, m_capacity);
    if (m_data)
        ::operator delete[](m_data);
    m_data = 0;
    m_size = 0;
    m_pos = 0;
    m_capacity = 0;
}
