#include <windows.h>
#include "QpcChrono.h"
#include "_imports_p09.h"
#include "_imports_g5.h"

BOOL QpcChrono::start()
{
    return QueryPerformanceCounter(&m_start);
}

int QpcChrono::stop()
{
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    __int64 elapsed = (now.QuadPart - m_start.QuadPart) * 1000000 / m_frequency.QuadPart;
    int result = (int)elapsed;
    start();
    return result;
}

int QpcChrono::lap()
{
    int result = stop();
    start();
    return result;
}
