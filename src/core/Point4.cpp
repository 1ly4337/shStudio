#include "Point4.h"
#include "iStream.hpp"
#include "oStream.hpp"
#include "String.hpp"

void Point4::read(iStream& in, DWORD fmt)
{
    x = in.readFloat();
    y = in.readFloat();
    z = in.readFloat();
    w = in.readFloat();
}

void Point4::write(oStream& out, DWORD fmt) const
{
    out << x << y << z << w;
}

bool Point4::toText(String& out, DWORD fmt) const
{
    out << L'(';
    out << x;
    out << L',';
    out << y;
    out << L',';
    out << z;
    out << L',';
    out << w;
    out << L')';
    return true;
}
