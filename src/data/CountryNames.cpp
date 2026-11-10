#include "CountryNames.h"

const wchar_t* countryName(UINT code)
{
    if (code == 156)
        return L"CHINA";
    if (code == 250)
        return L"FRANCE";
    if (code == 276)
        return L"GERMANY";
    if (code == 380)
        return L"ITALY";
    if (code == 410)
        return L"KOREA";
    if (code == 616)
        return L"POLAND";
    if (code == 792)
        return L"TURKEY";
    if (code != 840)
        return 0;
    return L"USA";
}

const wchar_t* countryCode(UINT code)
{
    if (code == 156)
        return L"CHN";
    if (code == 250)
        return L"FRC";
    if (code == 276)
        return L"GER";
    if (code == 380)
        return L"ITA";
    if (code == 410)
        return L"KOR";
    if (code == 616)
        return L"POL";
    if (code == 792)
        return L"TUR";
    if (code != 840)
        return 0;
    return L"USA";
}
