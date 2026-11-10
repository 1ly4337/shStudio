#pragma once
#include <windows.h>
#include "HTable.h"

extern HTable<unsigned int, HBITMAP> g_menuBitmaps;

bool initMenuBitmaps();

BOOL setMenuItemBitmap(HMENU menu, UINT item);
