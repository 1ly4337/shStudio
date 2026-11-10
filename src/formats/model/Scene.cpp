#include "Scene.h"
#include "Sequence.h"
#include "Point5.h"
#include <cmath>

Scene::Scene(int format)
{
    m_format = format;
    for (int i = 0; i < 10; ++i)
        m_ints[i] = 0;
    for (int i = 0; i < 23; ++i)
        m_floats[i] = 0;
    m_valueFC = 0;
    m_value100 = 0;
    m_valueD4 = 0;
    m_valueCC = 0;
    m_valueC8 = 0;
    m_valueB0 = 0;
    m_valueAC = 0;
    m_valueA8 = 0;
    m_value104 = 0;
    m_valueD0 = 0;
    m_pointCount = 0;
    m_keyCount = 0;
    m_valueD8 = 0;
    m_keys = 0;
    m_pairsB = 0;
    m_pairsA = 0;
    m_points = 0;
    m_valueDC = 0;
    m_pairCount = 0;
}

Scene::~Scene()
{
    if (m_pairsA)
        ::operator delete[](m_pairsA);
    if (m_pairsB)
        ::operator delete[](m_pairsB);
    if (m_keys)
        delete[] m_keys;
    if (m_points)
        delete[] m_points;
}

void Scene::read(iStream& in, DWORD fmt)
{
    m_name.read(in, 4, 949);
    for (int i = 0; i < 10; ++i)
        m_ints[i] = in.readInt();
    for (int i = 0; i < 23; ++i)
        m_floats[i] = in.readFloat();
    m_valueA8 = in.readInt();
    m_valueAC = in.readInt();
    m_valueB0 = in.readInt();
    m_color.read(in, 'file');
    m_valueC8 = in.readInt();
    m_valueCC = in.readInt();
    m_valueD0 = in.readFloat();
    m_valueD4 = in.readInt();
    if (m_format == 'EF3 ')
    {
        m_valueD8 = in.readInt();
        m_valueDC = in.readInt();
    }
    m_keyCount = in.readInt();
    if (m_keyCount > 0)
    {
        m_keys = new Point5[m_keyCount];
        for (int i = 0; i < m_keyCount; ++i)
            m_keys[i].read(in, fmt);
    }
    m_pairCount = in.readInt();
    if (m_pairCount > 0)
    {
        m_pairsA = (float*)::operator new[](((unsigned __int64)(unsigned)m_pairCount >> 30) != 0 ? (size_t)-1 : 4 * m_pairCount);
        m_pairsB = (float*)::operator new[](((unsigned __int64)(unsigned)m_pairCount >> 30) != 0 ? (size_t)-1 : 4 * m_pairCount);
        for (int i = 0; i < m_pairCount; ++i)
        {
            m_pairsA[i] = in.readFloat();
            m_pairsB[i] = in.readFloat();
        }
    }
    m_pointCount = in.readInt();
    if (m_pointCount > 0)
    {
        m_points = new fPoint[m_pointCount];
        for (int i = 0; i < m_pointCount; ++i)
            m_points[i].read(in, fmt);
    }
    m_valueFC = in.readInt();
    m_value100 = in.readInt();
    m_value104 = in.readFloat();
    m_value108 = in.readInt();
    if (fabsf(m_value104) < 1.0e-10f)
        m_value104 = 0.0f;
    int n = in.readInt();
    for (int i = 0; i < n; ++i)
        m_textures.add(in.readInt());
}

bool Scene::toText(String& out, DWORD fmt) const
{
    if (fmt == 'dump')
    {
        (out << L"name:" << L'\t').append(m_name.m_buffer, m_name.m_length) << L"\r\n";
    }
    for (int i = 0; i < 10; ++i)
        out << (long)m_ints[i] << L'\t';
    if (fmt == 'dump')
        out.newline();
    for (int i = 0; i < 23; ++i)
        out << m_floats[i] << L'\t';
    if (fmt == 'dump')
        out.newline();
    String& s = out << (long)m_valueA8 << L'\t' << (long)m_valueAC << L'\t' << (long)m_valueB0 << L'\t';
    m_color.toText(s, '----');
    s << L'\t';
    (s << (long)m_valueC8 << L'\t' << (long)m_valueCC << L'\t' << m_valueD0 << L'\t' << (long)m_valueD4).newline();
    if (fmt == 'dump')
    {
        for (int i = 0; i < m_keyCount; ++i)
        {
            m_keys[i].toText(out, '----');
            out.newline();
        }
        for (int i = 0; i < m_pairCount; ++i)
            (out << L'(' << m_pairsA[i] << L',' << m_pairsB[i] << L')').newline();
        for (int i = 0; i < m_pointCount; ++i)
            appendFPoint(out, &m_points[i]).newline();
        (out << (long)m_valueFC << L'\t' << (long)m_value100 << L'\t' << m_value104 << L'\t' << (long)m_value108).newline();
        int n = m_textures.m_count;
        out << L'(';
        for (int i = 0; i < n; ++i)
        {
            if (i < 0 || i >= m_textures.m_capacity)
                throw ArrayIndexOutOfBoundsException(i, m_textures.m_capacity);
            long* items = m_textures.m_items;
            out << L'\t' << (long)items[i];
        }
        out << L')';
        out.newline();
    }
    return true;
}

void Sequence::read(iStream& in, DWORD )
{
    m_name.read(in, 4, in.m_codePage);
    int n = in.readInt();
    for (int i = 0; i < n; ++i)
    {
        m_frames.add(in.readInt());
        m_delays.add(in.readFloat());
    }
}

bool Sequence::toText(String& out, DWORD ) const
{
    out.append(m_name.m_buffer, m_name.m_length) << L"\r\n";
    int n = m_frames.m_count;
    float total = 0.0f;
    for (int i = 0; i < n; ++i)
    {
        float delay = (i >= 0 && i < m_delays.m_capacity) ? m_delays.m_items[i] : 0.0f;
        long frame = (i >= 0 && i < m_frames.m_capacity) ? m_frames.m_items[i] : 0;
        ((out << L'\t' << frame) << L'\t' << delay).newline();
        total += (i >= 0 && i < m_delays.m_capacity) ? m_delays.m_items[i] : 0.0f;
    }
    (out << L"delay ms(?):\t" << total).newline();
    return true;
}
