#include "MonsterModel.h"
#include "Spot.h"

const wchar_t* const g_monsterAnimationNames[18] = {
    L"Walk    ", L"Run     ", L"Jmp/Atk1", L"Attack 2", L"Attack 3", L"Die     ",
    L"Breath  ", L"Damage  ", L"Idle    ",
    L"Attack 1", L"Attack 2", L"Attack 3", L"Die     ",
    L"Attack 1", L"Attack 2", L"Attack 3", L"Die     ", L"Binding ",
};

wchar_t g_monsterEffectsText[256];

void MonsterModel::Effects::assign(const Effects& src)
{
    deleteAll();
    int n = src.size();
    for (int i = 0; i < n; ++i)
    {
        const Effect* e = src.m_items[i];
        if (e)
            add(new Effect(*e));
    }
}

void MonsterModel::Effects::read(iStream& in, DWORD )
{
    int n = in.readInt();
    for (int i = 0; i < n; ++i)
    {
        Effect* e = new Effect;
        e->m_id = 0;
        e->m_id = in.peekInt();
        in.skip(4);
        e->m_value = in.peekInt();
        in.skip(4);
        add(e);
    }
}

bool MonsterModel::Effects::toText(String& out, DWORD ) const
{
    for (int i = 0; i < m_count; ++i)
    {
        const Effect* e = m_items[i];
        out << (long)e->m_id << L'\t' << (long)e->m_value;
        out.newline();
    }
    return true;
}

MonsterModel::MonsterModel(DWORD format, int id, int )
    : m_animations(32), m_vertices(32), m_textures(32)
{
    m_animations.m_allowDuplicates = true;
    m_vertices.m_allowDuplicates = true;
    m_textures.m_allowDuplicates = true;
    m_id = id;
    m_format = format;
    m_unk1 = 0xFF;
    m_height = 0.0f;
}

MonsterModel::~MonsterModel()
{
}

bool MonsterModel::copyFrom(const Streamable* src)
{
    const MonsterModel* p = dynamic_cast<const MonsterModel*>(src);
    if (!p)
        return false;
    assign(*p);
    return true;
}

void MonsterModel::assign(const MonsterModel& src)
{
    m_format = src.m_format;
    m_name = src.m_name;
    m_unk1 = src.m_unk1;
    m_height = src.m_height;
    m_animations = src.m_animations;
    m_vertices = src.m_vertices;
    m_textures = src.m_textures;
    m_effects.assign(src.m_effects);
}

const wchar_t* MonsterModel::animationName(long id) const
{
    String* s = m_animations.findById(id);
    if (s)
        return s->m_buffer;
    return L"";
}

const wchar_t* MonsterModel::getString(DWORD tag) const
{
    switch (tag)
    {
    case 'name':
        return m_name.m_buffer;
    case 'text':
        if (m_textures.m_count > 0 && m_textures.m_items[0])
            return m_textures.m_items[0]->m_buffer;
        return 0;
    case 'vert':
        if (m_vertices.m_count > 0 && m_vertices.m_items[0])
            return m_vertices.m_items[0]->m_buffer;
        return 0;
    case 'awlk': return animationName(0);
    case 'arun': return animationName(1);
    case 'ajmp': return animationName(2);
    case 'aat2': return animationName(3);
    case 'aat3': return animationName(4);
    case 'adie': return animationName(5);
    case 'abrt': return animationName(6);
    case 'admg': return animationName(7);
    case 'aidl': return animationName(8);
    case 'wat1': return animationName(9);
    case 'wat2': return animationName(10);
    case 'wat3': return animationName(11);
    case 'wdie': return animationName(12);
    case 'eat1': return animationName(13);
    case 'eat2': return animationName(14);
    case 'eat3': return animationName(15);
    case 'edie': return animationName(16);
    case 'elnk': return animationName(17);
    case 'effc':
    {
        memset(g_monsterEffectsText, 0, 256);
        int n = m_effects.size();
        if (!n)
            return L"";
        String list;
        for (int i = 0; i < n - 1; ++i)
            list << (long)m_effects.m_items[i]->m_value << ',';
        list << (long)m_effects.m_items[n - 1]->m_value;
        swprintf_s(g_monsterEffectsText, 256, L"%s", list.m_buffer);
        return g_monsterEffectsText;
    }
    }
    return 0;
}

int MonsterModel::getInt(DWORD tag) const
{
    switch (tag)
    {
    case 'indx': return m_id;
    case 'frmt': return (int)m_format;
    case 'unk1': return m_unk1;
    case 'hght': return *(const int*)&m_height;
    case '#txt': return m_textures.size();
    case '#ver': return m_vertices.size();
    case '#eff': return m_effects.size();
    }
    return 0;
}

bool MonsterModel::setString(DWORD tag, const wchar_t* value)
{
    if (tag != 'name')
        return false;
    m_name = value;
    return true;
}

bool MonsterModel::setInt(DWORD tag, int value)
{
    if (tag != 'unk1')
        return false;
    m_unk1 = (BYTE)value;
    return true;
}

void MonsterModel::reset()
{
    m_name.clear();
    m_animations.deleteAll();
    m_animations.m_ids.m_count = 0;
    m_vertices.deleteAll();
    m_vertices.m_ids.m_count = 0;
    m_textures.deleteAll();
    m_textures.m_ids.m_count = 0;
    m_unk1 = 0;
    m_height = 0.0f;
    m_effects.deleteAll();
}

static int p02_cmpInt(int a, int b)
{
    if (a != b)
        return 2 * (a >= b) - 1;
    return 0;
}

int MonsterModel::compare(const Node* other, DWORD tag) const
{
    const MonsterModel* p = dynamic_cast<const MonsterModel*>(other);
    switch (tag)
    {
    case 'indx': return p02_cmpInt(m_id, p->m_id);
    case 'name': return String::compareLogical(m_name, p->m_name);
    case 'unk1':
        if (m_unk1 != p->m_unk1)
            return m_unk1 < p->m_unk1 ? -1 : 1;
        return 0;
    case 'hght': return compareFloat(m_height, p->m_height);
    case '#txt': return p02_cmpInt(m_textures.size(), p->m_textures.size());
    case '#ver': return p02_cmpInt(m_vertices.size(), p->m_vertices.size());
    case '#eff': return p02_cmpInt(m_effects.size(), p->m_effects.size());
    default: return 0;
    }
}

void MonsterModel::read(iStream& in, DWORD fmt)
{
    if (fmt == 'clip')
    {
        m_id = in.readInt();
        m_format = (DWORD)in.readInt();
    }
    m_name.read(in, 4, in.m_codePage);
    m_unk1 = in.readByte();
    String s;
    int slots = (m_format != 'MO2 ') + 17;
    for (int i = 0; i < slots; ++i)
    {
        s.read(in, 4, (UINT)-1);
        if (s.m_length != 4 || _wcsnicmp(s.m_buffer, L"LOAD", 4))
            m_animations.add((long)i, s.m_buffer);
    }
    int n = in.peekInt();
    in.skip(4);
    for (; n > 0; --n)
    {
        s.read(in, 4, (UINT)-1);
        m_vertices.add(s.m_buffer);
        s.read(in, 4, (UINT)-1);
        m_textures.add(s.m_buffer);
    }
    m_height = in.readFloat();
    m_effects.read(in, fmt);
}

void MonsterModel::write(oStream& out, DWORD fmt) const
{
    if (fmt == 'clip')
        out << (unsigned long)m_id << (unsigned long)m_format;
    m_name.write(out, 4, out.m_codePage);
    out << (char)m_unk1;
    int slots = (m_format != 'MO2 ') + 17;
    for (int i = 0; i < slots; ++i)
    {
        const String* s = m_animations.findById(i);
        if (s)
            s->write(out, 4, (UINT)-1);
        else
        {
            out << (unsigned long)4;
            out.put("LOAD", 4);
        }
    }
    int n = m_vertices.size();
    out << (unsigned long)n;
    for (int i = 0; i < n; ++i)
    {
        m_vertices.at(i)->write(out, 4, (UINT)-1);
        m_textures.at(i)->write(out, 4, (UINT)-1);
    }
    out << m_height;
    out << (unsigned long)m_effects.m_count;
    for (int i = 0; i < m_effects.m_count; ++i)
    {
        const Effect* e = m_effects.m_items[i];
        out << (unsigned long)e->m_id << (unsigned long)e->m_value;
    }
}

bool MonsterModel::toText(String& out, DWORD fmt) const
{
    if ((fmt & 0x73716C00) == 0x73716C00)
        return false;
    out << L'#' << (long)m_id << L"\r\n";
    out << L"name:" << L'\t' << m_name << L"\r\n";
    out << L"unk1:" << L'\t' << (unsigned char)m_unk1 << L"\r\n";
    out << L"Animations:" << L"\r\n";
    for (int i = 0; i <= 8; ++i)
    {
        const String* s = m_animations.findById(i);
        if (s)
            out << g_monsterAnimationNames[i] << L'\t' << *s << L"\r\n";
    }
    out << L"Sounds:" << L"\r\n";
    for (int i = 9; i <= 12; ++i)
    {
        const String* s = m_animations.findById(i);
        if (s)
            (out << g_monsterAnimationNames[i] << L'\t' << *s).newline();
    }
    out << L"Effects:" << L"\r\n";
    for (int i = 13; i <= 17; ++i)
    {
        const String* s = m_animations.findById(i);
        if (s)
            (out << g_monsterAnimationNames[i] << L'\t' << *s).newline();
    }
    out << L"Shapes:" << L"\r\n";
    int n = m_vertices.size();
    for (int i = 0; i < n; ++i)
    {
        const wchar_t* texture = (i >= 0 && i < m_textures.m_count && m_textures.m_items[i])
                                     ? m_textures.m_items[i]->m_buffer : 0;
        const wchar_t* vertex = (i >= 0 && i < m_vertices.m_count && m_vertices.m_items[i])
                                    ? m_vertices.m_items[i]->m_buffer : 0;
        (out << vertex << L'\t' << texture).newline();
    }
    (out << L"Height:" << L'\t' << m_height).newline();
    if (m_effects.size() > 0)
    {
        out << L"Additional Effects:" << L"\r\n";
        m_effects.toText(out, 0);
    }
    out.newline();
    return true;
}
