#pragma once
#include "aStream.h"

class String;

class iStream : public virtual aStream
{
public:
    iStream(bool swapBytes = false, UINT codePage = 0) : aStream(swapBytes, codePage), m_flag(false) {}

    char           readByte();
    short          readShort();
    unsigned long  peekInt();
    unsigned long  readInt();
    unsigned long  readDword();
    __int64        readInt64();
    float          readFloat();
    void           read(void* dst, int size);
    void*          readAlloc(int size);
    void           read(aStream& dst, int size);
    void           gets(void* dst, int size);
    class FileTree* readFileTree();
    void           readBytes(class Bytes& dst, int size);

    bool m_flag;
};
