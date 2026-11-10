#include "dark_mode.h"
#include <commctrl.h>
#include <uxtheme.h>
#include <richedit.h>
#include <cstring>
#include <cwchar>

namespace {

const COLORREF kWindow  = RGB(32, 32, 32);
const COLORREF kFace    = RGB(43, 43, 43);
const COLORREF kText    = RGB(230, 230, 230);
const COLORREF kGray    = RGB(128, 128, 128);
const COLORREF kMenuHot = RGB(65, 65, 65);
const COLORREF kBorder  = RGB(80, 80, 80);

const UINT     ID_DARKMODE = 0xDA01;
const wchar_t  kRegKey[]   = L"Software\\shStudio 10";
const wchar_t  kOldRegKey[] = L"Software\\ilya.sh.studio";

bool  g_on = false;
bool  g_saved = false;
bool  g_restart = false;
HHOOK g_hook = 0;
HWND  g_main = 0;

typedef DWORD    (WINAPI* GetSysColor_t)(int);
typedef HBRUSH   (WINAPI* GetSysColorBrush_t)(int);
typedef int      (WINAPI* FillRect_t)(HDC, const RECT*, HBRUSH);
typedef COLORREF (WINAPI* SetColor_t)(HDC, COLORREF);
GetSysColor_t      realGetSysColor;
GetSysColorBrush_t realGetSysColorBrush;
FillRect_t         realFillRect;
SetColor_t         realSetTextColor;
SetColor_t         realSetBkColor;

enum PreferredAppMode { AppModeDefault, AppModeAllowDark, AppModeForceDark, AppModeForceLight };
typedef PreferredAppMode (WINAPI* SetPreferredAppMode_t)(PreferredAppMode);
typedef bool (WINAPI* AllowDarkModeForWindow_t)(HWND, bool);
typedef void (WINAPI* FlushMenuThemes_t)();
typedef void (WINAPI* RefreshImmersiveColorPolicyState_t)();
typedef HRESULT (WINAPI* DwmSetWindowAttribute_t)(HWND, DWORD, LPCVOID, DWORD);
AllowDarkModeForWindow_t pAllowDarkModeForWindow;
DwmSetWindowAttribute_t  pDwmSetWindowAttribute;

HBRUSH g_sysBrush[32];
struct BrushSlot { COLORREF color; HBRUSH brush; };
BrushSlot g_brushes[32];

HBRUSH brushFor(COLORREF c)
{
    for (BrushSlot& s : g_brushes)
    {
        if (s.brush && s.color == c)
            return s.brush;
        if (!s.brush)
        {
            s.color = c;
            s.brush = CreateSolidBrush(c);
            return s.brush;
        }
    }
    static HBRUSH last;
    if (last)
        DeleteObject(last);
    last = CreateSolidBrush(c);
    return last;
}

DWORD darkSysColor(int index)
{
    switch (index)
    {
    case COLOR_WINDOW:            return kWindow;
    case COLOR_WINDOWTEXT:
    case COLOR_BTNTEXT:
    case COLOR_MENUTEXT:
    case COLOR_CAPTIONTEXT:
    case COLOR_INFOTEXT:          return kText;
    case COLOR_BTNFACE:
    case COLOR_MENU:
    case COLOR_MENUBAR:           return kFace;
    case COLOR_GRAYTEXT:          return kGray;
    case COLOR_HIGHLIGHTTEXT:     return RGB(255, 255, 255);
    case COLOR_3DSHADOW:          return kBorder;
    case COLOR_3DHILIGHT:         return RGB(70, 70, 70);
    case COLOR_3DDKSHADOW:        return RGB(15, 15, 15);
    case COLOR_3DLIGHT:           return RGB(60, 60, 60);
    case COLOR_APPWORKSPACE:      return RGB(25, 25, 25);
    case COLOR_SCROLLBAR:         return RGB(50, 50, 50);
    case COLOR_INFOBK:            return RGB(50, 50, 50);
    case COLOR_WINDOWFRAME:       return kBorder;
    case COLOR_ACTIVEBORDER:
    case COLOR_INACTIVEBORDER:    return RGB(60, 60, 60);
    case COLOR_HOTLIGHT:          return RGB(100, 170, 255);
    case COLOR_MENUHILIGHT:       return kMenuHot;
    default:                      return realGetSysColor(index);
    }
}

DWORD WINAPI hkGetSysColor(int index)
{
    return g_on ? darkSysColor(index) : realGetSysColor(index);
}

HBRUSH WINAPI hkGetSysColorBrush(int index)
{
    if (!g_on || index < 0 || index >= 32)
        return realGetSysColorBrush(index);
    if (!g_sysBrush[index])
        g_sysBrush[index] = CreateSolidBrush(darkSysColor(index));
    return g_sysBrush[index];
}

int WINAPI hkFillRect(HDC dc, const RECT* rc, HBRUSH brush)
{
    UINT_PTR v = (UINT_PTR)brush;
    if (g_on && v >= 1 && v <= 31)
        brush = hkGetSysColorBrush((int)v - 1);
    return realFillRect(dc, rc, brush);
}

COLORREF WINAPI hkSetTextColor(HDC dc, COLORREF c)
{
    return realSetTextColor(dc, dmFg(c));
}

COLORREF WINAPI hkSetBkColor(HDC dc, COLORREF c)
{
    if (GetObjectType(dc) == OBJ_MEMDC)
        return realSetBkColor(dc, c);
    return realSetBkColor(dc, dmBk(c));
}

void patchImport(HMODULE module, const char* dll, const char* name, void* hook)
{
    BYTE* base = (BYTE*)module;
    IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)base;
    IMAGE_NT_HEADERS* nt = (IMAGE_NT_HEADERS*)(base + dos->e_lfanew);
    IMAGE_DATA_DIRECTORY& dir = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    if (!dir.VirtualAddress)
        return;
    for (IMAGE_IMPORT_DESCRIPTOR* imp = (IMAGE_IMPORT_DESCRIPTOR*)(base + dir.VirtualAddress); imp->Name; ++imp)
    {
        if (_stricmp((const char*)(base + imp->Name), dll) != 0)
            continue;
        IMAGE_THUNK_DATA* thunk = (IMAGE_THUNK_DATA*)(base + imp->FirstThunk);
        IMAGE_THUNK_DATA* orig = imp->OriginalFirstThunk ? (IMAGE_THUNK_DATA*)(base + imp->OriginalFirstThunk) : thunk;
        for (; orig->u1.AddressOfData; ++orig, ++thunk)
        {
            if (IMAGE_SNAP_BY_ORDINAL(orig->u1.Ordinal))
                continue;
            IMAGE_IMPORT_BY_NAME* ibn = (IMAGE_IMPORT_BY_NAME*)(base + orig->u1.AddressOfData);
            if (strcmp((const char*)ibn->Name, name) != 0)
                continue;
            DWORD old;
            if (VirtualProtect(&thunk->u1.Function, sizeof(thunk->u1.Function), PAGE_READWRITE, &old))
            {
                thunk->u1.Function = (decltype(thunk->u1.Function))(UINT_PTR)hook;
                VirtualProtect(&thunk->u1.Function, sizeof(thunk->u1.Function), old, &old);
            }
        }
    }
}

bool readSetting()
{
    DWORD v = 0, size = sizeof(v);
    if (RegGetValueW(HKEY_CURRENT_USER, kRegKey, L"DarkMode", RRF_RT_REG_DWORD, 0, &v, &size) == ERROR_SUCCESS)
        return v != 0;
    size = sizeof(v);
    if (RegGetValueW(HKEY_CURRENT_USER, kOldRegKey, L"DarkMode", RRF_RT_REG_DWORD, 0, &v, &size) == ERROR_SUCCESS)
        return v != 0;
    size = sizeof(v);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"AppsUseLightTheme", RRF_RT_REG_DWORD, 0, &v, &size) == ERROR_SUCCESS)
        return v == 0;
    return false;
}

void writeSetting(bool on)
{
    DWORD v = on ? 1 : 0;
    RegSetKeyValueW(HKEY_CURRENT_USER, kRegKey, L"DarkMode", REG_DWORD, &v, sizeof(v));
}

bool isDarkNeutral(COLORREF c)
{
    int r = GetRValue(c), g = GetGValue(c), b = GetBValue(c);
    int hi = r > g ? (r > b ? r : b) : (g > b ? g : b);
    int lo = r < g ? (r < b ? r : b) : (g < b ? g : b);
    return hi - lo < 30 && hi < 110;
}

LRESULT CALLBACK ctlColorProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR, DWORD_PTR)
{
    switch (msg)
    {
    case WM_CTLCOLORMSGBOX:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORBTN:
    case WM_CTLCOLORDLG:
    case WM_CTLCOLORSCROLLBAR:
    case WM_CTLCOLORSTATIC:
        if (g_on)
        {
            LRESULT r = DefSubclassProc(hwnd, msg, wParam, lParam);
            HDC dc = (HDC)wParam;
            HWND ctrl = (HWND)lParam;
            COLORREF bg = (msg == WM_CTLCOLOREDIT || msg == WM_CTLCOLORLISTBOX) ? kWindow : kFace;
            if (r)
            {
                if ((UINT_PTR)r >= 1 && (UINT_PTR)r <= 31)
                    r = (LRESULT)hkGetSysColorBrush((int)r - 1);
                LOGBRUSH lb;
                if (GetObjectW((HGDIOBJ)r, sizeof(lb), &lb) == sizeof(lb) && lb.lbStyle == BS_SOLID)
                {
                    COLORREF mapped = dmBk(lb.lbColor);
                    if (mapped != lb.lbColor)
                        r = (LRESULT)brushFor(mapped);
                    bg = mapped;
                }
            }
            else
            {
                r = (LRESULT)brushFor(bg);
            }
            if (isDarkNeutral(GetTextColor(dc)))
                realSetTextColor(dc, IsWindowEnabled(ctrl) ? kText : kGray);
            realSetBkColor(dc, bg);
            return r;
        }
        break;
    case WM_NOTIFY:
        if (g_on && lParam && ((NMHDR*)lParam)->code == NM_CUSTOMDRAW)
        {
            wchar_t cls[32] = {};
            GetClassNameW(((NMHDR*)lParam)->hwndFrom, cls, 31);
            if (!_wcsicmp(cls, WC_HEADERW))
            {
                NMCUSTOMDRAW* cd = (NMCUSTOMDRAW*)lParam;
                if (cd->dwDrawStage == CDDS_PREPAINT)
                    return CDRF_NOTIFYITEMDRAW;
                if (cd->dwDrawStage == CDDS_ITEMPREPAINT)
                {
                    realSetTextColor(cd->hdc, kText);
                    return CDRF_DODEFAULT;
                }
            }
        }
        break;
    case WM_NCDESTROY:
        RemoveWindowSubclass(hwnd, ctlColorProc, 1);
        break;
    }
    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK statusProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR, DWORD_PTR)
{
    switch (msg)
    {
    case WM_ERASEBKGND:
        if (g_on)
            return 1;
        break;
    case WM_PAINT:
        if (g_on)
        {
            PAINTSTRUCT ps;
            HDC dc = BeginPaint(hwnd, &ps);
            RECT client;
            GetClientRect(hwnd, &client);
            realFillRect(dc, &client, brushFor(kFace));
            HGDIOBJ oldFont = SelectObject(dc, (HFONT)SendMessageW(hwnd, WM_GETFONT, 0, 0));
            SetBkMode(dc, TRANSPARENT);
            realSetTextColor(dc, kText);
            int parts = (int)SendMessageW(hwnd, SB_GETPARTS, 0, 0);
            if (parts < 1)
                parts = 1;
            if (parts > 32)
                parts = 32;
            for (int i = 0; i < parts; ++i)
            {
                RECT pr;
                if (!SendMessageW(hwnd, SB_GETRECT, i, (LPARAM)&pr))
                    continue;
                wchar_t text[512] = {};
                if (LOWORD(SendMessageW(hwnd, SB_GETTEXTLENGTHW, i, 0)) < 511)
                    SendMessageW(hwnd, SB_GETTEXTW, i, (LPARAM)text);
                RECT tr = pr;
                tr.left += 4;
                tr.right -= 2;
                DrawTextW(dc, text, -1, &tr, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
                if (i + 1 < parts)
                {
                    RECT sep = { pr.right - 1, pr.top + 2, pr.right, pr.bottom - 2 };
                    realFillRect(dc, &sep, brushFor(kBorder));
                }
            }
            RECT top = { client.left, client.top, client.right, client.top + 1 };
            realFillRect(dc, &top, brushFor(kBorder));
            if (oldFont)
                SelectObject(dc, oldFont);
            EndPaint(hwnd, &ps);
            return 0;
        }
        break;
    case WM_NCDESTROY:
        RemoveWindowSubclass(hwnd, statusProc, 3);
        break;
    }
    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

const UINT WM_UAHDRAWMENU     = 0x0091;
const UINT WM_UAHDRAWMENUITEM = 0x0092;
struct UAHMENU { HMENU hmenu; HDC hdc; DWORD dwFlags; };
union UAHMENUITEMMETRICS
{
    struct { DWORD cx; DWORD cy; } rgsizeBar[2];
    struct { DWORD cx; DWORD cy; } rgsizePopup[4];
};
struct UAHMENUPOPUPMETRICS { DWORD rgcx[4]; DWORD fUpdateMaxWidths : 2; };
struct UAHMENUITEM { int iPosition; UAHMENUITEMMETRICS umim; UAHMENUPOPUPMETRICS umpm; };
struct UAHDRAWMENUITEM { DRAWITEMSTRUCT dis; UAHMENU um; UAHMENUITEM umi; };

void paintMenuBarLine(HWND hwnd)
{
    MENUBARINFO mbi = { sizeof(mbi) };
    if (!GetMenuBarInfo(hwnd, OBJID_MENU, 0, &mbi))
        return;
    RECT client;
    GetClientRect(hwnd, &client);
    MapWindowPoints(hwnd, 0, (POINT*)&client, 2);
    RECT win;
    GetWindowRect(hwnd, &win);
    OffsetRect(&client, -win.left, -win.top);
    RECT line = client;
    line.bottom = line.top;
    line.top--;
    HDC dc = GetWindowDC(hwnd);
    realFillRect(dc, &line, brushFor(kFace));
    ReleaseDC(hwnd, dc);
}

void relaunch()
{
    wchar_t path[MAX_PATH];
    if (!GetModuleFileNameW(0, path, MAX_PATH))
        return;
    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    if (CreateProcessW(path, 0, 0, 0, FALSE, 0, 0, 0, &si, &pi))
    {
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    }
}

void toggle(HWND hwnd)
{
    g_saved = !g_saved;
    writeSetting(g_saved);
    CheckMenuItem(GetMenu(hwnd), ID_DARKMODE, MF_BYCOMMAND | (g_saved ? MF_CHECKED : MF_UNCHECKED));
    if (g_saved == g_on)
        return;
    const wchar_t* text = g_saved ? L"Dark mode is on. Restart shStudio 10 now to apply it?"
                                  : L"Dark mode is off. Restart shStudio 10 now to apply it?";
    if (MessageBoxW(hwnd, text, L"shStudio 10", MB_YESNO | MB_ICONQUESTION) == IDYES)
    {
        g_restart = true;
        PostMessageW(hwnd, WM_CLOSE, 0, 0);
    }
}

bool menuBarMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT& result)
{
    switch (msg)
    {
    case WM_UAHDRAWMENU:
        if (g_on)
        {
            UAHMENU* um = (UAHMENU*)lParam;
            MENUBARINFO mbi = { sizeof(mbi) };
            GetMenuBarInfo(hwnd, OBJID_MENU, 0, &mbi);
            RECT win;
            GetWindowRect(hwnd, &win);
            RECT rc = mbi.rcBar;
            OffsetRect(&rc, -win.left, -win.top);
            rc.top -= 1;
            realFillRect(um->hdc, &rc, brushFor(kFace));
            result = TRUE;
            return true;
        }
        break;
    case WM_UAHDRAWMENUITEM:
        if (g_on)
        {
            UAHDRAWMENUITEM* item = (UAHDRAWMENUITEM*)lParam;
            wchar_t text[256] = {};
            MENUITEMINFOW mii = { sizeof(mii) };
            mii.fMask = MIIM_STRING;
            mii.dwTypeData = text;
            mii.cch = 255;
            GetMenuItemInfoW(item->um.hmenu, item->umi.iPosition, TRUE, &mii);
            UINT state = item->dis.itemState;
            COLORREF bg = (state & (ODS_HOTLIGHT | ODS_SELECTED)) ? kMenuHot : kFace;
            COLORREF fg = (state & (ODS_GRAYED | ODS_DISABLED | ODS_INACTIVE)) ? kGray : kText;
            UINT flags = DT_CENTER | DT_SINGLELINE | DT_VCENTER;
            if (state & ODS_NOACCEL)
                flags |= DT_HIDEPREFIX;
            realFillRect(item->um.hdc, &item->dis.rcItem, brushFor(bg));
            SetBkMode(item->um.hdc, TRANSPARENT);
            realSetTextColor(item->um.hdc, fg);
            DrawTextW(item->um.hdc, text, -1, &item->dis.rcItem, flags);
            result = TRUE;
            return true;
        }
        break;
    case WM_NCPAINT:
    case WM_NCACTIVATE:
        if (g_on)
        {
            result = DefSubclassProc(hwnd, msg, wParam, lParam);
            paintMenuBarLine(hwnd);
            return true;
        }
        break;
    }
    return false;
}

LRESULT CALLBACK menuBarProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR, DWORD_PTR)
{
    LRESULT r = 0;
    if (menuBarMsg(hwnd, msg, wParam, lParam, r))
        return r;
    if (msg == WM_NCDESTROY)
        RemoveWindowSubclass(hwnd, menuBarProc, 4);
    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK mainProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR, DWORD_PTR)
{
    LRESULT r = 0;
    if (menuBarMsg(hwnd, msg, wParam, lParam, r))
        return r;
    switch (msg)
    {
    case WM_COMMAND:
        if (LOWORD(wParam) == ID_DARKMODE && HIWORD(wParam) == 0)
        {
            toggle(hwnd);
            return 0;
        }
        break;
    case WM_NCDESTROY:
        RemoveWindowSubclass(hwnd, mainProc, 2);
        if (g_restart)
            relaunch();
        break;
    }
    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

bool menuTextMatches(HMENU bar, int pos, const wchar_t* word)
{
    wchar_t text[64] = {};
    GetMenuStringW(bar, pos, text, 63, MF_BYPOSITION);
    wchar_t clean[64] = {};
    int n = 0;
    for (const wchar_t* p = text; *p && n < 63; ++p)
        if (*p != L'&')
            clean[n++] = (wchar_t)towlower(*p);
    return wcsstr(clean, word) != 0;
}

void addMenuItem(HWND hwnd)
{
    HMENU bar = GetMenu(hwnd);
    if (!bar)
        return;
    int count = GetMenuItemCount(bar);
    HMENU target = 0;
    static const wchar_t* words[] = { L"view", L"option", L"tool", L"window" };
    for (const wchar_t* w : words)
    {
        for (int i = 0; i < count && !target; ++i)
            if (menuTextMatches(bar, i, w))
                target = GetSubMenu(bar, i);
        if (target)
            break;
    }
    UINT checked = g_saved ? MF_CHECKED : MF_UNCHECKED;
    if (target)
    {
        AppendMenuW(target, MF_SEPARATOR, 0, 0);
        AppendMenuW(target, MF_STRING | checked, ID_DARKMODE, L"Dark mode");
    }
    else
    {
        HMENU popup = CreatePopupMenu();
        AppendMenuW(popup, MF_STRING | checked, ID_DARKMODE, L"Dark mode");
        AppendMenuW(bar, MF_POPUP, (UINT_PTR)popup, L"&Theme");
    }
    DrawMenuBar(hwnd);
}

void setDarkTitleBar(HWND hwnd)
{
    if (!pDwmSetWindowAttribute)
        return;
    BOOL dark = TRUE;
    if (FAILED(pDwmSetWindowAttribute(hwnd, 20, &dark, sizeof(dark))))
        pDwmSetWindowAttribute(hwnd, 19, &dark, sizeof(dark));
}

void themeWindow(HWND hwnd)
{
    LONG style = GetWindowLongW(hwnd, GWL_STYLE);
    if (!g_main && !(style & WS_CHILD) && GetMenu(hwnd))
    {
        g_main = hwnd;
        addMenuItem(hwnd);
        SetWindowSubclass(hwnd, mainProc, 2, 0);
    }
    if (!g_on)
        return;
    if (hwnd != g_main && !(style & WS_CHILD) && GetMenu(hwnd))
        SetWindowSubclass(hwnd, menuBarProc, 4, 0);

    wchar_t cls[64] = {};
    GetClassNameW(hwnd, cls, 63);
    if (pAllowDarkModeForWindow)
        pAllowDarkModeForWindow(hwnd, true);
    if (!(style & WS_CHILD))
        setDarkTitleBar(hwnd);

    if (!_wcsicmp(cls, WC_TREEVIEWW))
    {
        SetWindowTheme(hwnd, L"DarkMode_Explorer", 0);
        TreeView_SetBkColor(hwnd, kWindow);
        TreeView_SetTextColor(hwnd, kText);
        TreeView_SetLineColor(hwnd, kBorder);
    }
    else if (!_wcsicmp(cls, WC_LISTVIEWW))
    {
        SetWindowTheme(hwnd, L"DarkMode_Explorer", 0);
        ListView_SetBkColor(hwnd, kWindow);
        ListView_SetTextBkColor(hwnd, kWindow);
        ListView_SetTextColor(hwnd, kText);
        if (HWND header = ListView_GetHeader(hwnd))
            SetWindowTheme(header, L"DarkMode_ItemsView", 0);
    }
    else if (!_wcsicmp(cls, WC_HEADERW))
        SetWindowTheme(hwnd, L"DarkMode_ItemsView", 0);
    else if (!_wcsicmp(cls, STATUSCLASSNAMEW))
        SetWindowSubclass(hwnd, statusProc, 3, 0);
    else if (!_wcsicmp(cls, UPDOWN_CLASSW))
        SetWindowTheme(hwnd, L"DarkMode_Explorer", 0);
    else if (!_wcsicmp(cls, L"Edit") || !_wcsicmp(cls, L"ComboBox") || !_wcsicmp(cls, WC_COMBOBOXEXW))
        SetWindowTheme(hwnd, L"DarkMode_CFD", 0);
    else if (!_wcsicmp(cls, L"ListBox") || !_wcsicmp(cls, L"ComboLBox") || !_wcsicmp(cls, L"ScrollBar")
             || !_wcsicmp(cls, TOOLTIPS_CLASSW))
        SetWindowTheme(hwnd, L"DarkMode_Explorer", 0);
    else if (!_wcsicmp(cls, L"Button"))
    {
        LONG type = style & BS_TYPEMASK;
        if (type == BS_PUSHBUTTON || type == BS_DEFPUSHBUTTON || type == BS_SPLITBUTTON || type == BS_DEFSPLITBUTTON)
            SetWindowTheme(hwnd, L"DarkMode_Explorer", 0);
        else if (type != BS_OWNERDRAW)
            SetWindowTheme(hwnd, L"", L"");
    }
    else if (!_wcsnicmp(cls, L"RichEdit", 8) || !_wcsnicmp(cls, L"RICHEDIT", 8))
    {
        SendMessageW(hwnd, EM_SETBKGNDCOLOR, 0, kWindow);
        CHARFORMAT2W cf = {};
        cf.cbSize = sizeof(cf);
        cf.dwMask = CFM_COLOR;
        cf.crTextColor = kText;
        SendMessageW(hwnd, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&cf);
        SendMessageW(hwnd, EM_SETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf);
    }
    else if (_wcsnicmp(cls, L"Sys", 3) && _wcsnicmp(cls, L"msctls_", 7) && _wcsicmp(cls, L"Static")
             && _wcsicmp(cls, TOOLBARCLASSNAMEW) && _wcsicmp(cls, REBARCLASSNAMEW))
    {
        SetWindowSubclass(hwnd, ctlColorProc, 1, 0);
    }
}

LRESULT CALLBACK callWndRetProc(int code, WPARAM wParam, LPARAM lParam)
{
    if (code == HC_ACTION)
    {
        CWPRETSTRUCT* m = (CWPRETSTRUCT*)lParam;
        if (m->message == WM_CREATE && m->lResult != -1)
            themeWindow(m->hwnd);
    }
    return CallNextHookEx(g_hook, code, wParam, lParam);
}

}

bool darkModeOn()
{
    return g_on;
}

COLORREF dmBk(COLORREF c)
{
    if (!g_on)
        return c;
    switch (c & 0xFFFFFF)
    {
    case 0xFFFFFF: return kWindow;
    case 0xF0F0F0: return kFace;
    case 0xEEEEEE: return RGB(50, 50, 50);
    case 0xD9D9D9: return RGB(70, 70, 70);
    case 0xFBD6B8: return RGB(38, 79, 120);
    case 0xDDAC84: return RGB(50, 95, 145);
    case 0xCEA27D: return RGB(60, 105, 160);
    case 0xD984DD: return RGB(120, 60, 120);
    case 0xFEF7F2: return RGB(40, 48, 58);
    case 0xFDEBDC: return RGB(42, 58, 78);
    case 0xFCE2CD: return RGB(45, 68, 95);
    case 0xFCDDFD: return RGB(70, 45, 70);
    }
    if (c == realGetSysColor(COLOR_WINDOW))
        return kWindow;
    if (c == realGetSysColor(COLOR_BTNFACE))
        return kFace;
    return c;
}

COLORREF dmFg(COLORREF c)
{
    if (!g_on)
        return c;
    if ((c & 0xFFFFFF) == 0 || c == realGetSysColor(COLOR_WINDOWTEXT) || c == realGetSysColor(COLOR_BTNTEXT))
        return kText;
    if ((c & 0xFFFFFF) == 0x979797)
        return RGB(160, 160, 160);
    return c;
}

void darkModeInit()
{
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    HMODULE gdi32 = GetModuleHandleW(L"gdi32.dll");
    realGetSysColor      = (GetSysColor_t)GetProcAddress(user32, "GetSysColor");
    realGetSysColorBrush = (GetSysColorBrush_t)GetProcAddress(user32, "GetSysColorBrush");
    realFillRect         = (FillRect_t)GetProcAddress(user32, "FillRect");
    realSetTextColor     = (SetColor_t)GetProcAddress(gdi32, "SetTextColor");
    realSetBkColor       = (SetColor_t)GetProcAddress(gdi32, "SetBkColor");
    if (!realGetSysColor || !realGetSysColorBrush || !realFillRect || !realSetTextColor || !realSetBkColor)
        return;

    g_saved = readSetting();
    g_on = g_saved;

    if (g_on)
    {
        if (HMODULE ux = LoadLibraryExW(L"uxtheme.dll", 0, LOAD_LIBRARY_SEARCH_SYSTEM32))
        {
            pAllowDarkModeForWindow = (AllowDarkModeForWindow_t)GetProcAddress(ux, MAKEINTRESOURCEA(133));
            SetPreferredAppMode_t setMode = (SetPreferredAppMode_t)GetProcAddress(ux, MAKEINTRESOURCEA(135));
            RefreshImmersiveColorPolicyState_t refresh =
                (RefreshImmersiveColorPolicyState_t)GetProcAddress(ux, MAKEINTRESOURCEA(104));
            FlushMenuThemes_t flush = (FlushMenuThemes_t)GetProcAddress(ux, MAKEINTRESOURCEA(136));
            if (setMode)
                setMode(AppModeForceDark);
            if (refresh)
                refresh();
            if (flush)
                flush();
        }
        if (HMODULE dwm = LoadLibraryExW(L"dwmapi.dll", 0, LOAD_LIBRARY_SEARCH_SYSTEM32))
            pDwmSetWindowAttribute = (DwmSetWindowAttribute_t)GetProcAddress(dwm, "DwmSetWindowAttribute");

        HMODULE self = GetModuleHandleW(0);
        patchImport(self, "user32.dll", "GetSysColor", (void*)&hkGetSysColor);
        patchImport(self, "user32.dll", "GetSysColorBrush", (void*)&hkGetSysColorBrush);
        patchImport(self, "user32.dll", "FillRect", (void*)&hkFillRect);
        patchImport(self, "gdi32.dll", "SetTextColor", (void*)&hkSetTextColor);
        patchImport(self, "gdi32.dll", "SetBkColor", (void*)&hkSetBkColor);
    }
    g_hook = SetWindowsHookExW(WH_CALLWNDPROCRET, callWndRetProc, 0, GetCurrentThreadId());
}
