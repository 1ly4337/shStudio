#pragma once
#include <windows.h>

class aStream
{
public:
    aStream(bool swapBytes = false, UINT codePage = 0)
        : m_size(0), m_pos(0), m_capacity(0), m_data(0), m_swapBytes(swapBytes), m_codePage(codePage) {}
    virtual ~aStream()
    {
        if (m_data)
            ::operator delete[](m_data);
        m_data = 0;
    }
    virtual int load(const wchar_t* fileName);
    virtual int save(const wchar_t* fileName);

    bool allocate(int size);
    void wipe();
    int  skip(int count);
    bool grow(int extra);
    bool assign(const void* data, int size);
    bool append(const void* data, int size);

    int   m_size;
    int   m_pos;
    int   m_capacity;
    BYTE* m_data;
    bool  m_swapBytes;
    UINT  m_codePage;
};
static_assert(sizeof(aStream) == 0x1C, "aStream size");
