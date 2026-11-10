#pragma once
#include <windows.h>
#include "HTable.h"

HFONT createCodePageFont(int codePage, int height, LPCWSTR faceName);
HFONT createStdFont(int index);
HFONT getStdFont(int index);
COLORREF getStdColor(int index);
HBRUSH getSolidBrush(COLORREF color);

extern HFONT g_stdFonts[7];
extern int   g_layoutCodePage;
extern int   g_logPixelsY;
extern int   g_stdFontPoints[7];
extern int   g_stdFontItalic[7];
extern int   g_stdFontWeight[7];
extern HTable<unsigned long, HBRUSH> g_brushCache;
