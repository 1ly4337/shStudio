#pragma once
#include <windows.h>
#include <commdlg.h>
#include <shobjidl.h>
#include "ida_compat.h"
#include "gdi_cache.h"
#include "gdiutil.h"
#include "imageio.h"
#include "DIBPixels.h"
#include "WinAppearance.h"
#include "RegKey.h"
#include "CtrlHelpers.h"
#include "Library.h"
#include "ContentProvider.h"
#include "Preferences.h"
#include "shellLink.h"

class Control;
class String;
class StudioFrame;

extern StudioFrame* g_studioFrame;

extern HINSTANCE hModule;
extern String g_appName;
