#pragma once
#include <windows.h>
#include <cstdint>
#include "String.hpp"
#include "aStream.h"
#include "iStream.h"
#include "oStream.h"
#include "Bytes.h"
#include "Exception.h"
#include "primitiveVector.h"
#include "_imports_w2.h"

inline void w2_throwOutOfMemory()
{
    OutOfMemoryException e;
    e.format(L"OutOfMemoryException: %li requested by %s", 1);
    throw e;
}

inline void w2_throwIndexOutOfBounds(int index)
{
    ArrayIndexOutOfBoundsException e;
    e.format(L"ArrayIndexOutOfBoundsException %li out of %li", index);
    throw e;
}

inline void w2_putByte(oStream& s, uint8_t b)
{
    aStream& a = static_cast<aStream&>(s);
    if (a.m_size + 65 > a.m_capacity) {
        if (!a.grow(1))
            w2_throwOutOfMemory();
    }
    a.m_data[a.m_size] = b;
    ++a.m_size;
}

inline void w2_rewind(iStream& s, int n)
{
    aStream& a = static_cast<aStream&>(s);
    int back = n + 4;
    if (n == -4) {
        a.m_pos = back;
    } else {
        int pos = a.m_pos;
        if (back <= pos)
            a.m_pos = pos - back;
    }
}

inline void w2_rewind0(iStream& s, int n)
{
    aStream& a = static_cast<aStream&>(s);
    int back = n + 4;
    if (n == -4) {
        a.m_pos = 0;
    } else {
        int pos = a.m_pos;
        if (back <= pos)
            a.m_pos = pos - back;
    }
}

inline int w2_readIntInline(iStream& s)
{
    int v = (int)s.peekInt();
    aStream& a = static_cast<aStream&>(s);
    int np = a.m_pos + 4;
    if (np > a.m_size)
        w2_throwIndexOutOfBounds(np);
    a.m_pos = np;
    return v;
}

inline void w2_strReplaceChar(String& s, wchar_t from, wchar_t to)
{
    for (unsigned i = 0; i < s.m_length; ++i) {
        if (s.m_buffer[i] == from)
            s.m_buffer[i] = to;
    }
}

template <class T> inline int w2_cmpS(T a, T b) { if (a == b) return 0; return a >= b ? 1 : -1; }
inline int w2_cmpU(uint8_t a, uint8_t b) { if (a == b) return 0; return a < b ? -1 : 1; }

template <class T>
inline primitiveVector<T>& primitiveVectorAssign(primitiveVector<T>& dst, const primitiveVector<T>& src)
{
    for (int i = 0; i < dst.m_capacity; ++i)
        dst.resetItem(&dst.m_items[i]);
    dst.m_count = 0;
    dst.m_growBy = src.m_growBy;
    dst.grow((src.m_count & 0xFFFFFFC0) + 64);
    for (int j = 0; j < src.m_count; ++j)
        dst.add(src.m_items[j]);
    return dst;
}
