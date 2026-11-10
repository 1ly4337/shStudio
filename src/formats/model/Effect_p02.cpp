#include "p02_common.h"
#include "Effect.h"
#include "Scene.h"
#include "Sequence.h"

String& appendStrings(String& out, const Strings& list)
{
    for (int i = 0; i < list.m_count; ++i)
    {
        String& s = out << (long)(i + 1) << L'\t';
        const String* item = list.m_items[i];
        s.append(item->m_buffer, item->m_length);
        s.newline();
    }
    return out;
}

void Effect::clear()
{
    m_scenes.deleteAll();
    m_sequences.deleteAll();
    m_names.deleteAll();
    m_names.m_ids.m_count = 0;
    m_textures.deleteAll();
    m_textures.m_ids.m_count = 0;
}

void Effect::read(iStream& in, DWORD fmt)
{
    clear();
    if (in.m_size - in.m_pos < 11)
        throw 0;
    DWORD signature[2] = { 0, 0 };
    in.read(signature, 3);
    if ((WORD)signature[0] == 0x4645 && ((BYTE*)signature)[2] == 'T')
        m_flags = 'EFT ';
    else
    {
        if ((WORD)signature[0] != 0x4645 || ((BYTE*)signature)[2] != '3')
            throw 1;
        m_flags = 'EF3 ';
    }
    String text;
    int n = in.readInt();
    for (int i = 0; i < n; ++i)
    {
        text.read(in, 4, in.m_codePage);
        m_names.add(text.m_buffer);
    }
    n = in.readInt();
    for (int i = 0; i < n; ++i)
    {
        text.read(in, 4, in.m_codePage);
        m_textures.add(text.m_buffer);
    }
    n = in.readInt();
    for (int i = 0; i < n; ++i)
    {
        Scene* scene = new Scene(m_flags);
        scene->read(in, fmt);
        m_scenes.add(scene);
    }
    n = in.readInt();
    for (int i = 0; i < n; ++i)
    {
        Sequence* sequence = new Sequence();
        sequence->read(in, fmt);
        m_sequences.add(sequence);
    }
    if (in.m_size - in.m_pos)
    {
        clear();
        throw 3;
    }
}

bool Effect::toText(String& out, DWORD fmt) const
{
    out << L".3DE files" << L"\r\n";
    appendStrings(out, m_names) << L"\r\n";
    out << L".DDS files" << L"\r\n";
    appendStrings(out, m_textures) << L"\r\n";
    int n = m_scenes.size();
    if (!n)
        return true;
    if (n == 1)
        out << L"1 scene";
    else
        (out << (long)n) << L" scenes";
    out << L"\r\n" << L"\r\n";
    for (int i = 0; i < n; ++i)
    {
        out.m_flags &= ~0x11u;
        out << L'#' << (long)i;
        if (fmt == 'dump')
        {
            ++out.m_indent;
            out << L'\t';
            out << L"\r\n";
        }
        else
            out << L'\t';
        out.m_flags |= 0x11;
        m_scenes.m_items[i]->toText(out, fmt);
        if (fmt == 'dump')
        {
            p02_unindent(out);
            out << L"\r\n";
        }
    }
    out.m_flags &= ~0x11u;
    int ns = m_sequences.size();
    (out.newline() << (long)ns << L" sequence(s)").newline();
    for (int i = 0; i < ns; ++i)
    {
        out.m_flags &= ~0x11u;
        out << L'#' << (long)i;
        ++out.m_indent;
        out << L'\t';
        out.newline();
        out.m_flags |= 0x11;
        m_sequences.m_items[i]->toText(out, fmt);
        p02_unindent(out);
        out.newline();
    }
    return true;
}
