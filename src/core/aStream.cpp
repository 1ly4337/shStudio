#include "aStream.h"
#include "Exception.h"
#include "String.hpp"
#include <cstdio>
#include <cstring>
#include <sys/stat.h>

int aStream::load(const wchar_t* fileName)
{
    if (m_data)
    {
        ::operator delete[](m_data);
        m_data = 0;
        m_size = 0;
        m_capacity = 0;
        m_pos = 0;
    }
    if (!fileName)
        return 160;
    FILE* file = 0;
    errno_t result = _wfopen_s(&file, fileName, L"rb");
    if (!result)
    {
        struct _stat64 st;
        if (_fstat64(_fileno(file), &st) >= 0)
        {
            int size = (int)st.st_size;
            if (st.st_size <= 0x7FFFFFFF)
            {
                if (m_capacity >= size + 1 || grow(size + 1 - m_capacity))
                {
                    if (size <= 0 || fread(m_data, size, 1, file) == 1)
                    {
                        m_size = size;
                        m_data[size] = 0;
                    }
                    else
                    {
                        result = ferror(file);
                        if (!result)
                            result = 30;
                    }
                }
                else
                    result = 8;
            }
            else
                result = 8;
        }
        else
            result = 6;
    }
    if (file)
        fclose(file);
    return result;
}

int aStream::save(const wchar_t* fileName)
{
    if (!fileName)
        return 161;
    String tempName(fileName, (unsigned)-1);
    tempName << ".tmp";
    int result;
    FILE* file = 0;
    if (_wfopen_s(&file, tempName.m_buffer, L"wb"))
        result = 161;
    else
    {
        size_t written = fwrite(m_data, m_size, 1, file);
        fflush(file);
        fclose(file);
        if (written == 1)
        {
            _wremove(fileName);
            _wrename(tempName.m_buffer, fileName);
            result = 0;
        }
        else
        {
            _wremove(tempName.m_buffer);
            result = 29;
        }
    }
    return result;
}

bool aStream::allocate(int size)
{
    if (size < 0)
        return false;
    if (m_data)
        ::operator delete[](m_data);
    m_size = 0;
    m_pos = 0;
    m_capacity = 0;
    m_data = (BYTE*)::operator new[](size + 1);
    if (!m_data)
        return false;
    m_size = size;
    m_capacity = size;
    return true;
}

void aStream::wipe()
{
    if (m_data && m_capacity)
        memset(m_data, 0, m_capacity);
    m_size = 0;
    m_pos = 0;
}

int aStream::skip(int count)
{
    int pos = count + m_pos;
    int size = m_size;
    if (pos > size)
        throw ArrayIndexOutOfBoundsException(pos, size);
    m_pos = pos;
    return size - pos;
}

bool aStream::grow(int extra)
{
    if (extra > 0)
    {
        int step = (extra & 0x7FFF0000) + 0x10000;
        BYTE* data = (BYTE*)::operator new[](step + m_capacity);
        memset(data, 0, step + m_capacity);
        if (m_size > 0 && m_data)
            memcpy(data, m_data, m_size);
        if (m_data)
            ::operator delete[](m_data);
        m_capacity += step;
        m_data = data;
    }
    return true;
}

bool aStream::assign(const void* data, int size)
{
    if (!data)
        return false;
    if (size < 1)
        return false;
    if (m_capacity < size && !grow(size - m_capacity))
        return false;
    memset(m_data, 0, m_capacity);
    memcpy(m_data, data, size);
    m_size = size;
    m_pos = 0;
    return true;
}

bool aStream::append(const void* data, int size)
{
    if (size < 0 || (size > 0 && !data) || (size + m_size + 64 > m_capacity && !grow(size)))
        return false;
    if (size)
    {
        memcpy(m_data + m_size, data, size);
        m_size += size;
    }
    return true;
}
