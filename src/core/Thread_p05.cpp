#include "p05_common.h"
#include "Thread.h"

void Thread::destroy(Thread* self)
{
    HANDLE h = self->m_handle;
    if (h) {
        bool running = self->m_running;
        self->m_stopRequested = true;
        if (running) {
            WaitForSingleObject(h, 2500);
            if (self->m_running) {
                TerminateThread(self->m_handle, 0);
                CloseHandle(self->m_handle);
            }
        }
        self->m_handle = 0;
        self->m_running = false;
    }
    operator delete(self);
}
