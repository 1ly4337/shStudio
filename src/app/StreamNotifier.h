#pragma once
#include <windows.h>

class StreamNotifier
{
public:
    virtual bool setStatus(LPCWSTR text) = 0;
    virtual bool progress(LPCWSTR text, int stepLow, int stepHigh) = 0;
    virtual LRESULT setRange(LPARAM max) = 0;
};
static_assert(sizeof(StreamNotifier) == 0x4, "StreamNotifier size");
