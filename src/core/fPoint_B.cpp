#include "fPoint.h"

float g_fPointDummy;

float& fPoint::at(int index)
{
    if (!index)
        return x;
    if (index == 1)
        return y;
    if (index != 2)
        return g_fPointDummy;
    return z;
}
