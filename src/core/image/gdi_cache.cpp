#include "gdi_cache.h"
#include "dark_mode.h"
#include "Window.h"
#include "Exception.h"
#include "NoSuchElementException.h"
#include "String.hpp"
#include "Preferences.h"
#include "_imports_p10.h"

HFONT g_stdFonts[7];
int   g_layoutCodePage;
int   g_logPixelsY;
int   g_stdFontPoints[7] = { 8, 8, 8, 7, 8, 7, 8 };
int   g_stdFontItalic[7] = { 0, 0, 1, 0, 0, 0, 0 };
int   g_stdFontWeight[7] = { 400, 600, 400, 400, 400, 600, 400 };

HTable<unsigned long, HBRUSH> g_brushCache(32, 32);

HFONT createCodePageFont(int codePage, int height, LPCWSTR faceName)
{
    DWORD charset = DEFAULT_CHARSET;
    if (codePage > 1250)
    {
        switch (codePage)
        {
        case 1251: charset = RUSSIAN_CHARSET; break;
        case 1253: charset = GREEK_CHARSET; break;
        case 1254: charset = TURKISH_CHARSET; break;
        case 1255: charset = HEBREW_CHARSET; break;
        case 1256: charset = ARABIC_CHARSET; break;
        case 1257: charset = BALTIC_CHARSET; break;
        case 1258: charset = VIETNAMESE_CHARSET; break;
        case 1361: charset = JOHAB_CHARSET; break;
        default: break;
        }
    }
    else if (codePage == 1250)
    {
        charset = EASTEUROPE_CHARSET;
    }
    else if (codePage > 874)
    {
        switch (codePage)
        {
        case 932: charset = SHIFTJIS_CHARSET; break;
        case 936: charset = GB2312_CHARSET; break;
        case 949: charset = HANGUL_CHARSET; break;
        case 950: charset = CHINESEBIG5_CHARSET; break;
        default: break;
        }
    }
    else if (codePage == 874)
    {
        charset = THAI_CHARSET;
    }
    else if (codePage > 866)
    {
        if (codePage == 869)
            charset = GREEK_CHARSET;
    }
    else if (codePage == 866)
    {
        charset = RUSSIAN_CHARSET;
    }
    else if (codePage)
    {
        if (codePage == 2)
            charset = MAC_CHARSET;
    }
    else
    {
        charset = ANSI_CHARSET;
    }
    HFONT font = CreateFontW(height, 0, 0, 0, FW_NORMAL, 0, 0, 0, charset, OUT_TT_PRECIS, 0,
                             CLEARTYPE_QUALITY, 0, faceName);
    if (!font)
    {
        g_layoutCodePage = 1252;
        return CreateFontW(height, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_TT_PRECIS, 0,
                           CLEARTYPE_QUALITY, 0, faceName);
    }
    return font;
}

HFONT createStdFont(int index)
{
    if (!g_logPixelsY)
    {
        HDC hdc = GetDC(0);
        g_logPixelsY = GetDeviceCaps(hdc, LOGPIXELSY);
        ReleaseDC(0, hdc);
    }
    int height = MulDiv(g_stdFontPoints[index], g_logPixelsY, 72);
    return CreateFontW(-height, 0, 0, 0, g_stdFontWeight[index], g_stdFontItalic[index], 0, 0, DEFAULT_CHARSET,
                       OUT_TT_PRECIS, 0, CLEARTYPE_QUALITY, 0, L"Tahoma");
}

HFONT getStdFont(int index)
{
    if (index < 0 || index >= 7)
    {
        index = 0;
    }
    else if (index == 6)
    {
        HFONT font = g_stdFonts[6];
        if (!font)
        {
            String faceName = g_preferences.getString(L"Layout", L"font name", L"Tahoma");
            int size = (int)g_preferences.getInt(L"Layout", L"font size", 13);
            g_layoutCodePage = (int)g_preferences.getInt(L"Layout", L"code page", 0);
            font = createCodePageFont(g_layoutCodePage, size, faceName.m_buffer);
            g_stdFonts[6] = font;
        }
        return font;
    }
    if (!g_stdFonts[index])
        g_stdFonts[index] = createStdFont(index);
    return g_stdFonts[index];
}

static COLORREF getStdColorBase(int index)
{
    if (index >= 100)
    {
        switch (index)
        {
        case 100: return 16504504;
        case 101: return 14527620;
        case 102: return 13542013;
        case 103: return 14277081;
        case 104: return 14255325;
        case 110: return 16709618;
        case 111: return 16640988;
        case 112: return 16573133;
        case 113: return 15658734;
        case 114: return 16571901;
        default: break;
        }
    }
    if (g_classicAppearance < 0)
        g_classicAppearance = isClassicAppearance() != 0;
    if (g_classicAppearance <= 0)
    {
        if (index == COLOR_ACTIVECAPTION)
            return 12164479;
        if (index == COLOR_BTNFACE)
            return 15790320;
    }
    return GetSysColor(index);
}

COLORREF getStdColor(int index)
{
    return dmBk(getStdColorBase(index));
}

static HBRUSH* brushCacheGet(unsigned long key)
{
    int i = 0;
    if (g_brushCache.m_count > 0)
    {
        while (g_brushCache.m_table[i].key != key)
        {
            if (++i >= g_brushCache.m_count)
                goto missing;
        }
        if (i != -1)
            return &g_brushCache.m_table[i].value;
    }
missing:
    NoSuchElementException e;
    e.format(L"NoSuchElementException: key %li doesn't exist", key);
    throw e;
}

HBRUSH getSolidBrush(COLORREF color)
{
    try
    {
        return *brushCacheGet(color);
    }
    catch (...)
    {
        HBRUSH brush = CreateSolidBrush(color);
        UnrealizeObject(brush);
        g_brushCache.set(color, brush);
        return brush;
    }
}
