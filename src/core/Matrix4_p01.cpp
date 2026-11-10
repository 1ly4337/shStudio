#include "Matrix4.h"
#include "iStream.hpp"
#include "oStream.hpp"
#include "String.hpp"

void Matrix4::read(iStream& in, DWORD fmt)
{
    in.read(m, 0x40);
}

void Matrix4::write(oStream& out, DWORD fmt) const
{
    out.put(m, 0x40);
}

bool Matrix4::toText(String& out, DWORD fmt) const
{
    if (fmt == 'code')
    {
        out << L"\r\n";
        for (int row = 0; row < 4; ++row)
        {
            out << L"\t{ ";
            out << m[row * 4 + 0];
            out << L", ";
            out << m[row * 4 + 1];
            out << L", ";
            out << m[row * 4 + 2];
            out << L", ";
            out << m[row * 4 + 3];
            if (row < 3)
            {
                out << L" },";
                out << L"\r\n";
            }
            else
                out << L" }";
        }
    }
    else
    {
        if (fmt == 'icod')
        {
            for (int row = 0; row < 4; ++row)
            {
                out << L"{ ";
                out << m[row * 4 + 0];
                out << L", ";
                out << m[row * 4 + 1];
                out << L", ";
                out << m[row * 4 + 2];
                out << L", ";
                out << m[row * 4 + 3];
                out << (row < 3 ? L" }, " : L" }");
            }
            return true;
        }
        for (int row = 0; row < 4; ++row)
        {
            out << m[row * 4 + 0];
            out << L'\t';
            out << m[row * 4 + 1];
            out << L'\t';
            out << m[row * 4 + 2];
            out << L'\t';
            out << m[row * 4 + 3];
            if (row < 3)
                out << L"\r\n";
        }
    }
    out << L"\r\n";
    return true;
}
