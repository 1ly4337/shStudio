#include "fPoint.h"
#include <cmath>

fPoint fPoint::operator-(const fPoint& rhs) const
{
    fPoint r;
    r.x = x - rhs.x;
    r.y = y - rhs.y;
    r.z = z - rhs.z;
    return r;
}

fPoint fPoint::cross(const fPoint& rhs) const
{
    fPoint r;
    r.x = y * rhs.z - z * rhs.y;
    r.y = rhs.x * z - x * rhs.z;
    r.z = x * rhs.y - rhs.x * y;
    return r;
}

fPoint fPoint::unitCross(const fPoint& rhs) const
{
    fPoint c = cross(rhs);
    double inv;
    if (c.x * c.x + c.y * c.y + c.z * c.z >= 0.00000001f)
        inv = 1.0 / sqrt((double)(c.x * c.x + c.y * c.y + c.z * c.z));
    else
        inv = 100000000.0;
    float f = (float)inv;
    c.x = f * c.x;
    c.y = f * c.y;
    c.z = c.z * f;
    fPoint r;
    r.x = c.x;
    r.y = c.y;
    r.z = c.z;
    return r;
}
