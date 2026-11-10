#pragma once
#include <windows.h>
#include <commctrl.h>
#include "ida_compat.h"
#include "gdi_cache.h"
#include "CtrlHelpers.h"
#include "gdiutil.h"

extern int dword_5A0120;
extern int dword_5A0124;

#include "Data_Item.h"

int sub_43CBF0(wchar_t* buffer, const wchar_t* format, ...);
