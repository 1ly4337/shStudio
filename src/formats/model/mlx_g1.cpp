#include "mlx_g1.h"
#include "_imports_p07.h"

#pragma warning(disable : 4066 4498)

const wchar_t* const g_mlxPartNames[4] = { L"upper", L"lower", L"foot", L"hand" };
const wchar_t* const g_mlxColumnTitles[6] = { L"Model", L"Name", L"Upper", L"Lower", L"Boots", L"Hands" };
const int g_mlxColumnWidths[6] = { 45, 90, 150, 150, 150, 150 };
const DWORD g_mlxColumnIds[6] = { 'indx', 'name', 'vrUp', 'vrLo', 'vrFt', 'vrHa' };

static inline int packRaceCode(const wchar_t* code)
{
    int value = 0;
    for (int i = 0; i < 4; ++i)
    {
        if (!code)
            break;
        unsigned short c = *code;
        if (!c)
            break;
        value = c + (value << 8);
        ++code;
    }
    return value;
}

const wchar_t* mlxRaceDescription(const wchar_t* code)
{
    switch (packRaceCode(code))
    {
    case 'humm': return L"Human man mage (male priest)";
    case 'humf': return L"Human man fighter (fighter or defender)";
    case 'elwr': return L"Elf woman ranger (archer or ranger)";
    case 'elmr': return L"Elf man ranger (archer or ranger)";
    case 'elwm': return L"Elf woman mage (female mage)";
    case 'elmm': return L"Elf man mage (male mage)";
    case 'dewr': return L"Deatheaer woman ranger (female hunter)";
    case 'dewf': return L"Deatheater woman fighter (warrior or guardian)";
    case 'demf': return L"Deatheater man fighter (warrior or guardian)";
    case 'demr': return L"Deatheater man ranger (male hunter)";
    case 'vimr': return L"Vail man ranger (male assassin)";
    case 'vimm': return L"Vail man mage (pagan or oracle)";
    case 'tyro': return L"Tyross";
    case 'huwf': return L"Human woman fighter (fighter or defender)";
    case 'huwm': return L"Human woman mage (female priest)";
    case 'viwm': return L"Vail woman mage (pagan or oracle)";
    case 'viwr': return L"Vail woman ranger (female assassin)";
    case 'vlmf': return L"Vueltas man fighter";
    default:     return L"Unknown";
    }
}

int mlxRaceIndex(const wchar_t* code)
{
    if (!code)
        return -1;
    if (wcslen(code) != 4)
        return -1;
    switch (packRaceCode(code))
    {
    case 'humf': case 'dewf': case 'demf': case 'huwf':
        return 0;
    case 'elwr': case 'elmr': case 'vimr': case 'viwr':
        return 2;
    case 'dewr': case 'demr':
        return 3;
    case 'elwm': case 'elmm': case 'vimm': case 'viwm':
        return 4;
    case 'humm': case 'huwm':
        return 5;
    case 'tyro':
        return 6;
    case 'vlmf':
        return 7;
    default:
        return -1;
    }
}
