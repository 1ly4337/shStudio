#pragma once
#include <windows.h>
#include <commctrl.h>
#include "ida_compat.h"

class String;
class TreeList;
class FolderRoot;
class aStream;
class iStream;

#include "AES.h"

#include "ContentProvider.h"
#include "Preferences.h"
#include "gdi_cache.h"
#include "WinAppearance.h"
extern int       g_classicAppearance;
extern HCURSOR   hCursor;
extern HCURSOR   g_arrowCursor;
extern HINSTANCE hModule;
extern BYTE    unk_5467DC[16];
extern BYTE    unk_5467EC[16];
extern WCHAR   g_productName[16];
extern WCHAR   g_caption[24];
