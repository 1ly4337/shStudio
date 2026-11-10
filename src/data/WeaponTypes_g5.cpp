#include "WeaponTypes_g5.h"
#include "_imports_p07.h"

const wchar_t* weaponTypeName(unsigned int type)
{
    const wchar_t* result;
    switch (type)
    {
    case 1:    result = L"1-hand Sword"; break;
    case 2:    result = L"2-hands Sword"; break;
    case 3:    result = L"1-hand Axe"; break;
    case 4:    result = L"2-hands Axe"; break;
    case 5:    result = L"Dual Sword or Axe"; break;
    case 6:    result = L"Spear"; break;
    case 7:    result = L"1-hand Blunt"; break;
    case 8:    result = L"2-hands Blunt"; break;
    case 9:    result = L"Reverse Sword"; break;
    case 0xA:  result = L"Dagger"; break;
    case 0xB:  result = L"Javelin"; break;
    case 0xC:  result = L"Staff"; break;
    case 0xD:  result = L"Bow"; break;
    case 0xE:  result = L"Cross-bow"; break;
    case 0xF:  result = L"Knuckle"; break;
    case 0x13:
    case 0x22: result = L"Shield"; break;
    case 0x2D: result = L"1-hand Sword, 2nd set"; break;
    case 0x2E: result = L"2-hands Sword, 2nd set"; break;
    case 0x2F: result = L"1-hand Axe, 2nd set"; break;
    case 0x30: result = L"2-hands Axe, 2nd set"; break;
    case 0x31: result = L"Dual Swords, 2nd set"; break;
    case 0x32: result = L"Dual Axes, 2nd set"; break;
    case 0x33:
    case 0x34: result = L"Spears, 2nd set"; break;
    case 0x35:
    case 0x36: result = L"1-hand Blunt, 2nd set"; break;
    case 0x37:
    case 0x38: result = L"2-hands Blunt, 2nd set"; break;
    case 0x39: result = L"Reverse Sword, 2nd set"; break;
    case 0x3A: result = L"Dagger, 2nd set"; break;
    case 0x3B: result = L"Javelin, 2nd set"; break;
    case 0x3C:
    case 0x3D: result = L"Staff, 2nd set"; break;
    case 0x3E:
    case 0x3F: result = L"Bow, 2nd set"; break;
    case 0x40: result = L"Cross-bow, 2nd set"; break;
    case 0x41: result = L"Knuckle, 2nd set"; break;
    case 0x45:
    case 0x54: result = L"Shield, 2nd set"; break;
    default:   result = 0; break;
    }
    return result;
}
