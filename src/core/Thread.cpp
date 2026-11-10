#include <windows.h>
#include "Thread.h"
#include "_imports_p09.h"
#include "_imports_g5.h"

bool Thread::suspend()
{
    if (!m_handle)
        return false;
    if (m_running)
    {
        if (SuspendThread(m_handle) == (DWORD)-1)
            return false;
        m_running = false;
    }
    return true;
}

bool Thread::resume()
{
    if (!m_handle)
        return false;
    if (!m_running)
    {
        if (ResumeThread(m_handle) == (DWORD)-1)
            return false;
        m_running = true;
    }
    return true;
}

void Thread::stop(int timeoutMs)
{
    if (!m_handle)
        return;
    m_stopRequested = true;
    if (m_running)
    {
        if (timeoutMs > 0)
            WaitForSingleObject(m_handle, timeoutMs);
        if (m_running)
        {
            TerminateThread(m_handle, 0);
            CloseHandle(m_handle);
        }
    }
    m_handle = 0;
    m_running = false;
}

DWORD WINAPI Thread::threadProc(LPVOID param)
{
    Thread* thread = (Thread*)param;
    if (thread)
    {
        thread->m_running = true;
        thread->run();
        thread->m_running = false;
        thread->m_handle = 0;
        Thread::destroy(thread);
    }
    return 0;
}
