#include "Faces.h"
#include "Face.h"
#include "iStream.h"
#include "String.hpp"
#include "_imports_p10.h"

void Faces::read(iStream& in, DWORD fmt)
{
    deleteAll();
    aStream& buffer = in;
    if (buffer.m_size - buffer.m_pos < 4)
        throw 1;
    int count = (int)in.readInt();
    if (count < 0 || 6 * count > buffer.m_size - buffer.m_pos)
        throw 2;
    for (int i = 0; i < count; ++i)
    {
        Face* face = new Face((short)i);
        static_cast<Streamable*>(face)->read(in, fmt);
        add(face);
    }
}

bool Faces::toText(String& out, DWORD fmt) const
{
    bool ok = true;
    if (fmt == 'type')
    {
        out << L"typedef struct _Faces {\r\n\tunsigned short\t\ti1, i2, i3;\r\n} Faces;\r\n\r\n";
        out.m_flags &= ~1u;
        out << L"Faces faces[/* " << (long)m_count << L" */] = ";
        out.m_flags |= 1u;
        return ok;
    }
    if (fmt == 'code')
    {
        out << L'{';
        ++out.m_indent;
        out << L'\t';
        out << L"\r\n";
        int i = 0;
        if (m_count - 1 > 0)
        {
            do
            {
                if (!ok)
                    goto failed;
                ok = static_cast<const Streamable*>(m_items[i])->toText(out, 'code');
                out << L',';
                out << L"\r\n";
                ++i;
            } while (i < m_count - 1);
            if (!ok)
                goto failed;
        }
        if (static_cast<const Streamable*>(m_items[m_count - 1])->toText(out, 'code'))
            ok = true;
        else
        failed:
            ok = false;
        --out.m_indent;
        if (out.m_buffer && out.m_buffer[out.m_length - 1] == L'\t')
            out.m_buffer[out.m_length] = 0;
        if (out.m_buffer && out.m_buffer[out.m_length - 1] == L'\t')
        {
            --out.m_length;
            out.m_buffer[out.m_length] = 0;
        }
        String& line = out << L"\r\n";
        line << L'}';
        if (out.m_indent == 0)
        {
            out << L';';
            out << L"\r\n" << L"\r\n";
        }
        else
        {
            out << L',';
            out << L"\r\n";
        }
        return ok;
    }
    for (int i = 0; i < m_count && ok; ++i)
        ok = static_cast<const Streamable*>(m_items[i])->toText(out, fmt);
    return ok;
}
