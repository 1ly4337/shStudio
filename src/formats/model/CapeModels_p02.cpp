#include "p02_common.h"
#include "CapeModels.h"
#include "CapeData.h"

static void p02_readName128(iStream& in, String& name)
{
    char buffer[132];
    in.read(buffer, 128);
    buffer[128] = 0;
    int len = (int)strlen(buffer);
    if (name.reserve(len))
    {
        for (int k = 0; k < len; ++k)
            name.m_buffer[name.m_length + k] = (wchar_t)(short)buffer[k];
        name.m_length += len;
    }
}

static void p02_writeName128(oStream& out, const Strings& list, int i)
{
    char buffer[128];
    memset(buffer, 0, sizeof(buffer));
    if (i >= 0 && i < list.m_count)
    {
        const String* s = list.m_items[i];
        const wchar_t* text = s ? s->m_buffer : 0;
        if (text)
        {
            int len = (int)wcslen(text);
            if (len > 128)
                len = 128;
            for (int k = 0; k < len; ++k)
                buffer[k] = (char)text[k];
        }
    }
    out.put(buffer, 128);
}

bool CapeModels::isEmpty() const
{
    return m_capeFiles.size() == 0;
}

void CapeModels::clear()
{
    m_capeFiles.deleteAll();
    m_capeFiles.m_ids.m_count = 0;
    m_shoulderFiles.deleteAll();
    m_shoulderFiles.m_ids.m_count = 0;
    m_capes.deleteAll();
}

void CapeModels::read(iStream& in, DWORD )
{
    clear();
    int n = in.readInt();
    for (int i = 0; i < n; ++i)
    {
        in.peekInt();
        in.skip(4);
        String name;
        p02_readName128(in, name);
        m_capeFiles.add(name.m_buffer);
    }
    n = in.peekInt();
    in.skip(4);
    for (int i = 0; i < n; ++i)
    {
        CapeData* cape = new CapeData(i, 0);
        cape->read(in, 'file');
        m_capes.add(cape);
    }
    n = in.peekInt();
    in.skip(4);
    for (int i = 0; i < n; ++i)
    {
        in.peekInt();
        in.skip(4);
        String name;
        p02_readName128(in, name);
        m_shoulderFiles.add(name.m_buffer);
    }
}

void CapeModels::write(oStream& out, DWORD ) const
{
    int n = m_capeFiles.size();
    out << (unsigned long)n;
    for (int i = 0; i < n; ++i)
    {
        out << (unsigned long)i;
        p02_writeName128(out, m_capeFiles, i);
    }
    n = m_capes.size();
    out << (unsigned long)n;
    for (int j = 0; j < n; ++j)
        m_capes.get(j)->write(out, 'file');
    n = m_shoulderFiles.size();
    out << (unsigned long)n;
    for (int i = 0; i < n; ++i)
    {
        out << (unsigned long)i;
        p02_writeName128(out, m_shoulderFiles, i);
    }
}

bool CapeModels::toText(String& out, DWORD ) const
{
    out << L"mantle vertices:\n";
    int n = m_capeFiles.size();
    for (int i = 0; i < n; ++i)
    {
        const String* s = (i >= 0 && i < m_capeFiles.m_count) ? m_capeFiles.m_items[i] : 0;
        (out << (s ? s->m_buffer : (const wchar_t*)0)).newline();
    }
    out << L"shoulders vertices:\n";
    n = m_shoulderFiles.size();
    for (int i = 0; i < n; ++i)
    {
        const String* s = (i >= 0 && i < m_shoulderFiles.m_count) ? m_shoulderFiles.m_items[i] : 0;
        (out << (s ? s->m_buffer : (const wchar_t*)0)).newline();
    }
    out << L"settings:\n";
    n = m_capes.size();
    for (int i = 0; i < n; ++i)
        m_capes.get(i)->toText(out, 'dump');
    return true;
}
