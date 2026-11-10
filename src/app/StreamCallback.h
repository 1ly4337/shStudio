#pragma once
#include <windows.h>

class StreamCallback
{
public:
    virtual LRESULT onStreamProgress(WPARAM pos, LPARAM range) = 0;
};
