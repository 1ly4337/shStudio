#include "AniStep.h"

void AniStep::read(iStream& in, DWORD fmt)
{
    m_id = in.readInt();
    m_matrix.read(in, 'file');
    int rotationCount = in.readInt();
    m_rotationCount = rotationCount;
    if (rotationCount > 0)
    {
        m_rotations = new Rotation[rotationCount];
        for (int i = 0; i < m_rotationCount; ++i)
            m_rotations[i].read(in, fmt);
    }
    int translationCount = in.readInt();
    m_translationCount = translationCount;
    if (translationCount > 0)
    {
        m_translations = new Translation[translationCount];
        for (int i = 0; i < m_translationCount; ++i)
            m_translations[i].read(in, fmt);
    }
}

void AniStep::write(oStream& out, DWORD fmt) const
{
    m_matrix.write(out << (unsigned long)m_id, 'file');
    out << (unsigned long)m_rotationCount;
    for (int i = 0; i < m_rotationCount; ++i)
        m_rotations[i].write(out, fmt);
    out << (unsigned long)m_translationCount;
    for (int i = 0; i < m_translationCount; ++i)
        m_translations[i].write(out, fmt);
}

bool AniStep::toText(String& out, DWORD ) const
{
    out << (long)m_id << L'\t' << (long)m_rotationCount << L'\t' << (long)m_translationCount;
    return true;
}
