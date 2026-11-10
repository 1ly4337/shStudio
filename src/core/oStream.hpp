#pragma once
#include "aStream.h"

class iStream;

class oStream : public virtual aStream
{
public:
    oStream(bool swapBytes = false, UINT codePage = 0) : aStream(swapBytes, codePage), m_writeNul(true) {}

    oStream& put(const void* data, long size);
    oStream& operator<<(const wchar_t* text);
    oStream& operator<<(short v);
    oStream& operator<<(char v);
    oStream& operator<<(unsigned short v);
    oStream& operator<<(unsigned long v);
    oStream& operator<<(long v)  { return *this << (unsigned long)v; }
    oStream& operator<<(int v)   { return *this << (unsigned long)v; }
    oStream& operator<<(unsigned int v) { return *this << (unsigned long)v; }
    oStream& operator<<(__int64 v);
    oStream& operator<<(float v);
    int      put(iStream& src, int size);

    bool m_writeNul;
};
