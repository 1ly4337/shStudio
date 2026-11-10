#include "p04_common.h"
#include "TacticsZone.h"
#include "String.hpp"
#include <typeinfo>

class Scaler;
class GDC;

int TacticsZone::getInt(DWORD tag) const
{
    switch (tag)
    {
    case 'indx':
        return FIELD(int, this, 0x10);
    case 'map ':
        return FIELD(BYTE, this, 0x5C);
    case 'unk0':
        return FIELD(BYTE, this, 0x20);
    case 'unk1':
        return FIELD(BYTE, this, 0x21);
    case 'unk2':
        return FIELD(BYTE, this, 0x22);
    case 'psX1':
        return *FIELD(int*, this, 0x24);
    case 'psY1':
        return *FIELD(int*, this, 0x28);
    case 'psZ1':
        return *FIELD(int*, this, 0x2C);
    case 'psX2':
        return FIELD(int, this, 0x30);
    case 'psY2':
        return FIELD(int, this, 0x34);
    case 'psZ2':
        return FIELD(int, this, 0x38);
    case 'psX3':
        return FIELD(int, this, 0x3C);
    case 'psY3':
        return FIELD(int, this, 0x40);
    case 'psZ3':
        return FIELD(int, this, 0x44);
    case 'psX4':
        return FIELD(int, this, 0x48);
    case 'psY4':
        return FIELD(int, this, 0x4C);
    case 'psZ4':
        return FIELD(int, this, 0x50);
    case 'unk3':
        return FIELD(int, this, 0x54);
    case 'unk4':
        return FIELD(int, this, 0x58);
    case 'unk5':
        return FIELD(BYTE, this, 0x5D);
    }
    return 0;
}

bool TacticsZone::setString(DWORD tag, const wchar_t* value)
{
    if (tag != 'desc')
        return false;
    FIELD(String, this, 0x60) = value;
    return true;
}

bool TacticsZone::setInt(DWORD tag, int value)
{
    switch (tag)
    {
    case 'unk0':
        FIELD(BYTE, this, 0x20) = (BYTE)value;
        return true;
    case 'unk1':
        FIELD(BYTE, this, 0x21) = (BYTE)value;
        return true;
    case 'unk2':
        FIELD(BYTE, this, 0x22) = (BYTE)value;
        return true;
    case 'psX1':
        *FIELD(int*, this, 0x24) = value;
        return true;
    case 'psY1':
        *FIELD(int*, this, 0x28) = value;
        return true;
    case 'psZ1':
        *FIELD(int*, this, 0x2C) = value;
        return true;
    case 'psX2':
        FIELD(int, this, 0x30) = value;
        return true;
    case 'psY2':
        FIELD(int, this, 0x34) = value;
        return true;
    case 'psZ2':
        FIELD(int, this, 0x38) = value;
        return true;
    case 'psX3':
        FIELD(int, this, 0x3C) = value;
        return true;
    case 'psY3':
        FIELD(int, this, 0x40) = value;
        return true;
    case 'psZ3':
        FIELD(int, this, 0x44) = value;
        return true;
    case 'psX4':
        FIELD(int, this, 0x48) = value;
        return true;
    case 'psY4':
        FIELD(int, this, 0x4C) = value;
        return true;
    case 'psZ4':
        FIELD(int, this, 0x50) = value;
        return true;
    case 'unk3':
        FIELD(int, this, 0x54) = value;
        return true;
    case 'unk4':
        FIELD(int, this, 0x58) = value;
        return true;
    case 'map ':
        FIELD(BYTE, this, 0x5C) = (BYTE)value;
        return true;
    case 'unk5':
        FIELD(BYTE, this, 0x5D) = (BYTE)value;
        return true;
    default:
        return false;
    }
}

static inline int compareBytes(BYTE a, BYTE b)
{
    if (a != b)
        return a < b ? -1 : 1;
    return 0;
}

static inline int compareInts(int a, int b)
{
    if (a != b)
        return 2 * (a >= b) - 1;
    return 0;
}

int TacticsZone::compare(const Node* other, DWORD tag) const
{
    const TacticsZone& o = dynamic_cast<const TacticsZone&>(*other);
    if (tag == 'unk0')
        return compareBytes(FIELD(BYTE, this, 0x20), FIELD(BYTE, &o, 0x20));
    if (tag == 'unk1')
        return compareBytes(FIELD(BYTE, this, 0x21), FIELD(BYTE, &o, 0x21));
    if (tag == 'unk2')
        return compareBytes(FIELD(BYTE, this, 0x22), FIELD(BYTE, &o, 0x22));
    if (tag == 'psX1')
    {
        float a = *FIELD(float*, this, 0x24), b = *FIELD(float*, &o, 0x24);
        if (a != b)
            return 2 * (b <= a) - 1;
        return 0;
    }
    if (tag == 'psY1')
    {
        float a = *FIELD(float*, this, 0x28), b = *FIELD(float*, &o, 0x28);
        if (a == b)
            return 0;
        return 2 * (b <= a) - 1;
    }
    if (tag == 'psZ1')
        return compareFloat(*FIELD(float*, this, 0x2C), *FIELD(float*, &o, 0x2C));
    if (tag == 'psX2')
        return compareFloat(FIELD(float, this, 0x30), FIELD(float, &o, 0x30));
    if (tag == 'psY2')
        return compareFloat(FIELD(float, this, 0x34), FIELD(float, &o, 0x34));
    if (tag == 'psZ2')
        return compareFloat(FIELD(float, this, 0x38), FIELD(float, &o, 0x38));
    if (tag == 'psX3')
        return compareFloat(FIELD(float, this, 0x3C), FIELD(float, &o, 0x3C));
    if (tag == 'psY3')
        return compareFloat(FIELD(float, this, 0x40), FIELD(float, &o, 0x40));
    if (tag == 'psZ3')
        return compareFloat(FIELD(float, this, 0x44), FIELD(float, &o, 0x44));
    if (tag == 'psX4')
        return compareFloat(FIELD(float, this, 0x48), FIELD(float, &o, 0x48));
    if (tag == 'psY4')
        return compareFloat(FIELD(float, this, 0x4C), FIELD(float, &o, 0x4C));
    if (tag == 'psZ4')
        return compareFloat(FIELD(float, this, 0x50), FIELD(float, &o, 0x50));
    if (tag == 'unk3')
        return compareInts(FIELD(int, this, 0x54), FIELD(int, &o, 0x54));
    if (tag == 'unk4')
        return compareInts(FIELD(int, this, 0x58), FIELD(int, &o, 0x58));
    if (tag == 'map ')
        return compareBytes(FIELD(BYTE, this, 0x5C), FIELD(BYTE, &o, 0x5C));
    if (tag == 'unk5')
        return compareBytes(FIELD(BYTE, this, 0x5D), FIELD(BYTE, &o, 0x5D));
    return 0;
}

bool TacticsZone::toText(String& out, DWORD fmt) const
{
    if (fmt == 'sql ')
        return false;
    wchar_t sep = p04_separator(fmt);

    out << (unsigned char)FIELD(BYTE, this, 0x20) << sep
        << (unsigned char)FIELD(BYTE, this, 0x21) << sep
        << (unsigned char)FIELD(BYTE, this, 0x22);
    out.m_flags |= SF_PAD_SPACES | SF_FIXED2;
    out << sep;
    out << *FIELD(float*, this, 0x24) << sep
        << *FIELD(float*, this, 0x2C) << sep
        << FIELD(float, this, 0x30) << sep
        << FIELD(float, this, 0x38) << sep
        << FIELD(float, this, 0x3C) << sep
        << FIELD(float, this, 0x44) << sep
        << FIELD(float, this, 0x48) << sep
        << FIELD(float, this, 0x50);
    out.m_flags &= ~(unsigned)(SF_PAD_SPACES | SF_FIXED2);
    out << sep;
    out << (unsigned char)FIELD(BYTE, this, 0x5C);
    out << sep;
    out << FIELD(String, this, 0x60);
    return true;
}

void TacticsZone::draw(GDC* dc)
{
    Scaler* scaler = FIELD(Scaler*, this, 0x80);
    if (!scaler)
        return;

    COLORREF color = 0xFFFF80;
    if ((GetWindowLongW(FIELD(HWND, this, 0xC), GWL_STYLE) & 0xF) == 0)
        color = 0x0000FF;
    HPEN pen = CreatePen(PS_SOLID, 1, color);
    HGDIOBJ old = SelectObject(FIELD(HDC, dc, 0x4), pen);
    if (FIELD(HGDIOBJ, dc, 0x8))
        DeleteObject(old);
    else
        FIELD(HGDIOBJ, dc, 0x8) = old;

    POINT pt;
    POINT* p = P04_VCALL(scaler, 0x0, POINT*, POINT*, float, float)(scaler, &pt, *FIELD(float*, this, 0x24), *FIELD(float*, this, 0x2C));
    MoveToEx(FIELD(HDC, dc, 0x4), p->x - FIELD(int, this, 0x84), p->y - FIELD(int, this, 0x88), 0);
    scaler = FIELD(Scaler*, this, 0x80);
    p = P04_VCALL(scaler, 0x0, POINT*, POINT*, float, float)(scaler, &pt, FIELD(float, this, 0x30), FIELD(float, this, 0x38));
    LineTo(FIELD(HDC, dc, 0x4), p->x - FIELD(int, this, 0x84), p->y - FIELD(int, this, 0x88));
    scaler = FIELD(Scaler*, this, 0x80);
    p = P04_VCALL(scaler, 0x0, POINT*, POINT*, float, float)(scaler, &pt, FIELD(float, this, 0x3C), FIELD(float, this, 0x44));
    LineTo(FIELD(HDC, dc, 0x4), p->x - FIELD(int, this, 0x84), p->y - FIELD(int, this, 0x88));
    scaler = FIELD(Scaler*, this, 0x80);
    p = P04_VCALL(scaler, 0x0, POINT*, POINT*, float, float)(scaler, &pt, FIELD(float, this, 0x48), FIELD(float, this, 0x50));
    LineTo(FIELD(HDC, dc, 0x4), p->x - FIELD(int, this, 0x84), p->y - FIELD(int, this, 0x88));
    scaler = FIELD(Scaler*, this, 0x80);
    p = P04_VCALL(scaler, 0x0, POINT*, POINT*, float, float)(scaler, &pt, *FIELD(float*, this, 0x24), *FIELD(float*, this, 0x2C));
    LineTo(FIELD(HDC, dc, 0x4), p->x - FIELD(int, this, 0x84), p->y - FIELD(int, this, 0x88));
}

LPRECT TacticsZone::calcRect(LPRECT rc, Scaler* scaler)
{
    float x3 = FIELD(float, this, 0x3C);
    float x4 = FIELD(float, this, 0x48);
    FIELD(Scaler*, this, 0x80) = scaler;
    float minX34 = (x4 <= x3) ? x4 : x3;
    float x2 = FIELD(float, this, 0x30);
    float x1 = *FIELD(float*, this, 0x24);
    float maxX = x1;
    float cur = x1;
    float minX = (x2 <= x1) ? x2 : x1;
    if (minX34 <= minX)
        minX = minX34;
    if (x3 <= x4)
        x3 = x4;
    if (x1 <= x2)
    {
        cur = x2;
        maxX = x2;
    }
    if (cur <= x3)
        maxX = x3;

    float z3 = FIELD(float, this, 0x44);
    float z4 = FIELD(float, this, 0x50);
    float minZ34 = (z4 <= z3) ? z4 : z3;
    float z2 = FIELD(float, this, 0x38);
    float z1 = *FIELD(float*, this, 0x2C);
    float minZ = (z2 <= z1) ? z2 : z1;
    if (minZ34 <= minZ)
        minZ = minZ34;
    if (z3 <= z4)
        z3 = z4;
    if (z1 <= z2)
        z1 = z2;
    if (z1 <= z3)
        z1 = z3;

    POINT topLeft;
    POINT* p = P04_VCALL(scaler, 0x0, POINT*, POINT*, float, float)(scaler, &topLeft, minX, z1);
    FIELD(int, this, 0x84) = p->x;
    FIELD(int, this, 0x88) = p->y;
    Scaler* sc = FIELD(Scaler*, this, 0x80);
    POINT bottomRight;
    P04_VCALL(sc, 0x0, POINT*, POINT*, float, float)(sc, &bottomRight, maxX, minZ);
    int right = bottomRight.x;
    int bottom = bottomRight.y;
    rc->left = FIELD(int, this, 0x84);
    rc->right = right;
    rc->bottom = bottom;
    rc->top = FIELD(int, this, 0x88);
    if (FIELD(int, this, 0x84) < right)
        right = FIELD(int, this, 0x84);
    FIELD(int, this, 0x84) = right;
    if (FIELD(int, this, 0x88) < bottom)
        bottom = FIELD(int, this, 0x88);
    FIELD(int, this, 0x88) = bottom;
    InflateRect(rc, 1, 1);
    return rc;
}
