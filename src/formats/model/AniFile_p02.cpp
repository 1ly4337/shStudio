#include "p02_common.h"
#include "AniFile.h"
#include "AniStep.h"

void AniFile::clear()
{
    m_steps.deleteAll();
}

void AniFile::read(iStream& in)
{
    m_steps.deleteAll();
    m_header = (int)in.readDword();
    m_version = in.readShort();
    m_unk = in.readShort();
    m_flags = in.readShort();
    if (m_header != 0)
        OutputDebugStringW(L"header NOT null");
    if (m_unk != 0)
        OutputDebugStringW(L"Unk NOT null");
    try
    {
        for (;;)
        {
            AniStep* step = new AniStep();
            step->read(in, 0);
            m_steps.add(step);
        }
    }
    catch (...)
    {
    }
}

String& AniFile::dump(String& out)
{
    out << (unsigned long)m_header << L'\t' << m_version << L'\t' << m_unk << L'\t' << m_flags << L"\r\n";
    int n = m_steps.size();
    for (int i = 0; i < n; ++i)
    {
        out << L"{ ";
        m_steps.get(i)->toText(out, 0);
        out << L" }" << L"\r\n";
    }
    return out;
}
