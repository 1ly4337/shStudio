#include "SkillText.h"
#include "String.hpp"
#include "_imports_w1.h"
#include <cstdio>
#include <cstdarg>

wchar_t g_skillTypeTextBuf[128];

const wchar_t* getSkillTypeName(short typeDetail)
{
    if (typeDetail > 1062)
        return L"unknown";
    if (typeDetail == 1062)
        return L"Cure of Goddess";
    switch (typeDetail)
    {
    case 0:   return L"None";
    case 1:   return L"Weapon Power Up";
    case 2:   return L"Backpack";
    case 4:   return L"Passive Defense";
    case 5:   return L"Interpretation";
    case 6:   return L"Potential Defense";
    case 8:   return L"Weapon Mastery";
    case 10:  return L"Defense Power Up";
    case 21:  return L"Unique Hit Attack";
    case 22:  return L"Multiple Hit Attack";
    case 23:  return L"Eraser";
    case 25:  return L"HP, MP, SP Reduction";
    case 27:  return L"Energy Drain";
    case 41:  return L"Healing";
    case 42:  return L"Dispel";
    case 43:  return L"Sacrifice";
    case 61:  return L"Periodical Debuff";
    case 62:  return L"Subtracting Debuff";
    case 63:  return L"Flite";
    case 64:  return L"Stun";
    case 65:  return L"Immobilize";
    case 66:  return L"Sleep";
    case 67:  return L"Prevent Attack";
    case 68:  return L"Remove Attribute";
    case 69:  return L"Taunt";
    case 72:  return L"Provoke";
    case 74:  return L"Energy Blackhole";
    case 75:  return L"Mental Storm, Confusion";
    case 76:  return L"Soul Menace";
    case 77:  return L"Mental Storm, Distortion";
    case 78:  return L"unknow (mobs)";
    case 81:  return L"Periodical Heal";
    case 82:  return L"Buff, Noss";
    case 83:  return L"Damage Reflection";
    case 84:  return L"Fire thorn";
    case 85:  return L"Block Magic Attack";
    case 86:  return L"Block Shooting Attack";
    case 87:  return L"Health Assistant";
    case 88:  return L"Stealth";
    case 90:  return L"Evolution";
    case 91:  return L"Ability Exchange";
    case 92:  return L"Untouchable";
    case 93:  return L"Detection";
    case 94:  return L"Elemental Attack";
    case 95:  return L"Elemental Protection";
    case 96:  return L"Dance of Death";
    case 97:  return L"Mass Ambush";
    case 98:  return L"Etain Shield";
    case 101: return L"Trap";
    case 102: return L"Firebush";
    case 121: return L"Resurrection";
    case 122: return L"Soul Burn";
    case 123: return L"Scouting";
    case 125: return L"Town Portal";
    case 126: return L"Mental Impact";
    default:  return L"unknown";
    }
}

int swprintf128(wchar_t* buffer, const wchar_t* format, ...)
{
    va_list args;
    va_start(args, format);
    int r = vswprintf_s(buffer, 0x80, format, args);
    va_end(args);
    return r;
}

String& appendTabbedTriple(String& s, const unsigned char* v)
{
    s << v[0] << L'\t' << v[1] << L'\t' << v[2];
    return s;
}
