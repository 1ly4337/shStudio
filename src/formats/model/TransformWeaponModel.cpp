#include "TransformWeaponModel.h"
#include "iStream.h"
#include "oStream.h"
#include "String.hpp"
#include <typeinfo>

TransformWeaponModel::TransformWeaponModel()
    : Node()
{
    m_type = 0;
    m_weapon1 = 0;
    m_weapon = 0;
}

int TransformWeaponModel::getInt(DWORD tag) const
{
    switch (tag)
    {
    case 'type':
        return m_type;
    case 'wpn ':
        return m_weapon;
    case 'wpn1':
        return m_weapon1;
    }
    return 0;
}

bool TransformWeaponModel::setInt(DWORD tag, int value)
{
    switch (tag)
    {
    case 'type':
        m_type = value;
        return true;
    case 'wpn ':
        m_weapon = value;
        return true;
    case 'wpn1':
        m_weapon1 = value;
        return true;
    default:
        return false;
    }
}

void TransformWeaponModel::reset()
{
    m_weapon1 = 0;
    m_weapon = 0;
}

int TransformWeaponModel::compare(const Node* other, DWORD tag) const
{
    const TransformWeaponModel& o = dynamic_cast<const TransformWeaponModel&>(*other);
    switch (tag)
    {
    case 'type':
        if (m_type != o.m_type)
            return 2 * (m_type >= o.m_type) - 1;
        return 0;
    case 'wpn ':
        if (m_weapon != o.m_weapon)
            return 2 * (m_weapon >= o.m_weapon) - 1;
        return 0;
    case 'wpn1':
        if (m_weapon1 != o.m_weapon1)
            return 2 * (m_weapon1 >= o.m_weapon1) - 1;
        return 0;
    default:
        return 0;
    }
}

void TransformWeaponModel::assign(const TransformWeaponModel* other)
{
    m_type = other->m_type;
    m_weapon = other->m_weapon;
    m_weapon1 = other->m_weapon1;
}

bool TransformWeaponModel::copyFrom(const Streamable* src)
{
    const TransformWeaponModel* model = dynamic_cast<const TransformWeaponModel*>(src);
    if (!model)
        return false;
    assign(model);
    return true;
}

void TransformWeaponModel::read(iStream& in, DWORD fmt)
{
    if (fmt == 'file')
        throw (wchar_t*)L"This file requires DB format";
    if (fmt == 'flnt')
    {
        m_type = (int)in.readInt64();
        m_weapon = (int)in.readInt64();
        m_weapon1 = (int)in.readInt64();
    }
}

void TransformWeaponModel::write(oStream& out, DWORD fmt) const
{
    if (fmt == 'file')
        throw (wchar_t*)L"This file requires DB format";
    if (fmt == 'flnt')
        out << (__int64)m_type << (__int64)m_weapon << (__int64)m_weapon1;
}

bool TransformWeaponModel::toText(String& out, DWORD fmt) const
{
    wchar_t sep = p04_separator(fmt);
    if (fmt != '----')
        return false;
    out << (long)m_type << sep << (long)m_weapon << sep << (long)m_weapon1 << L"\r\n";
    return true;
}
