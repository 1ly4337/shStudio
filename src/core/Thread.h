#pragma once
#include <windows.h>

class Thread
{
public:
    Thread() : m_handle(0), m_threadId(0), m_stackSize(0x8000), m_creationFlags(0),
               m_running(false), m_stopRequested(false) {}
    ~Thread() { stop(2500); }

    static void destroy(Thread* self);

    virtual bool suspend();
    virtual bool resume();
    virtual void stop(int timeoutMs);
    virtual void run() = 0;

    HANDLE start()
    {
        m_handle = CreateThread(0, m_stackSize, threadProc, this, m_creationFlags, &m_threadId);
        return m_handle;
    }

    static DWORD WINAPI threadProc(LPVOID param);

public:
    HANDLE m_handle;
    DWORD  m_threadId;
    DWORD  m_stackSize;
    DWORD  m_creationFlags;
    bool   m_running;
    bool   m_stopRequested;
};
static_assert(sizeof(Thread) == 0x18, "Thread size");
