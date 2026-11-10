#pragma once
#include <windows.h>
#include <commctrl.h>
#include "ida_compat.h"
#include "gdiutil.h"
#include "imageio.h"
#include "CtrlHelpers.h"
#include "gdi_cache.h"
#include "AlertDial.h"

class Bytes;
class Popup;
class Window;
class glViewer;

extern HINSTANCE hModule;
class glView; extern glView* g_activeGlView;
extern int dword_5A0120;
extern int dword_5A0124;
class Application; extern Application* g_mainFrame;
