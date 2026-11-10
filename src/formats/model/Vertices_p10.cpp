#include "Vertices.h"
#include "Vertex.h"
#include "String.hpp"
#include "_imports_p10.h"

bool Vertices::toText(String& out, DWORD fmt) const
{
    bool ok = true;
    if (fmt == 'type')
    {
        if (m_count > 0)
            ok = m_items[0]->toText(out, fmt);
        out.m_flags &= ~1u;
        out << L"Vertices vertices[/* " << (long)m_count << L" */] = ";
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
                    break;
                out << L"{ ";
                ok = m_items[i]->toText(out, 'code');
                out << L" }," << L"\r\n";
                ++i;
            } while (i < m_count - 1);
        }
        out << L"{ ";
        if (ok && m_items[m_count - 1]->toText(out, 'code'))
            ok = true;
        else
            ok = false;
        out << L" }";
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
        ok = m_items[i]->toText(out, fmt);
    return ok;
}
