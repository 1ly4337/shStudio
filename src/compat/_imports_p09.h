#pragma once
#include <windows.h>
#include <commctrl.h>
#include "ida_compat.h"

#include "gdiutil.h"
#include "imageio.h"
#include "gdi_cache.h"
#include "menus.h"
#include "FileDialogs.h"
#include "shellLink.h"
#include "clipboard_A.h"
#include "AlertDial.h"

class String;
class Bytes;
class iStream;
class FileTree;
class TreeList;
class Window;

extern HCURSOR hCursor;
extern HCURSOR g_arrowCursor;
extern HWND g_hMainWnd;
class Application; extern Application* g_mainFrame;
class StudioFrame; extern StudioFrame* g_studioFrame;

extern HWND g_hMainWnd;
extern HCURSOR hCursor;
extern HINSTANCE hModule;

extern int __fastcall sub_41D430(__int16 bcd);

class String;

extern HINSTANCE hModule;
