#include "fPoint.h"
#include "iStream.hpp"
#include "oStream.hpp"
#include "String.hpp"

void fPoint::read(iStream& in, DWORD fmt)
{
    x = in.readFloat();
    y = in.readFloat();
    z = in.readFloat();
}

void fPoint::write(oStream& out, DWORD fmt) const
{
    out << x << y << z;
}

bool fPoint::toText(String& out, DWORD fmt) const
{
    out << L'(';
    out << x;
    out << L',';
    out << y;
    out << L',';
    out << z;
    out << L')';
    return true;
}
