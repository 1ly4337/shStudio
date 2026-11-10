#include "p02_common.h"
#include "fPoints.h"

bool fPoints::read(iStream& in, DWORD fmt)
{
    if (in.m_size - in.m_pos < 4)
        return false;
    int n = (int)in.readInt();
    if (n < 0 || n > ((in.m_size - in.m_pos) >> 2) / 3)
        return false;
    for (; n > 0; --n)
    {
        fPoint* p = new fPoint();
        p->read(in, fmt);
        add(p);
    }
    return true;
}
