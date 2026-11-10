#include "Point5.h"

void Point5::read(iStream& in, DWORD )
{
    a = in.readFloat();
    b = in.readFloat();
    c = in.readFloat();
    d = in.readFloat();
    e = in.readFloat();
}

void Point5::write(oStream& out, DWORD ) const
{
    out << a << b << c << d << e;
}

bool Point5::toText(String& out, DWORD ) const
{
    out << L'(' << a << L',' << b << L',' << c << L',' << d << L',' << e << L')';
    return true;
}
