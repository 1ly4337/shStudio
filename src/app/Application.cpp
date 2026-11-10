#include "Application.h"
#include "Doc.h"
#include "ImageView.h"
#include "TextView.h"
#include "FileTree.h"
#include "String.hpp"
#include "iStream.hpp"
#include "Bytes.h"
#include "glView.h"
#include "Preferences.h"
#include <commctrl.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <gdiplus.h>
#include "menus.h"
#include "clipboard_release.h"
#include "Modal.h"
#include "dragdrop.h"
#include "_imports_p10.h"

HINSTANCE hModule = GetModuleHandleW(0);
String g_appName;
HCURSOR g_arrowCursor = LoadCursorW(0, IDC_ARROW);
HCURSOR g_handCursor = LoadCursorW(0, IDC_HAND);
HCURSOR hCursor = LoadCursorW(0, IDC_WAIT);

HWND g_hMainWnd = 0;
Application* g_mainFrame = 0;
HBRUSH g_aboutBrush = 0;
const wchar_t* g_lastLoadError = 0;
ULONG_PTR g_gdiplusToken = 0;

Application::Application(HINSTANCE hInstance)
{
    hModule = hInstance;
    INITCOMMONCONTROLSEX icc;
    icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icc.dwICC = 0xFFFF;
    InitCommonControlsEx(&icc);
    registerClipboardFormats();
    m_hAccel = 0;
    m_active = false;
}

int Application::run()
{
    MSG msg;
    int result;
    while (true)
    {
        while (!PeekMessageW(&msg, 0, 0, 0, PM_NOREMOVE))
        {
            glView* view = g_activeGlView;
            if (!g_activeGlView)
                break;
            if (m_active && g_activeGlView)
            {
                view->processKeys();
                if (view->render())
                    view->swapBuffers();
                view->idle();
            }
            else
            {
                Sleep(100);
            }
        }
        result = GetMessageW(&msg, 0, 0, 0);
        if (!result || result == -1)
            break;
        if (!m_hAccel || !TranslateAcceleratorW(m_hWnd, m_hAccel, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
    return result;
}

bool Application::resolvePath(String* path)
{
    WIN32_FIND_DATAW findData;
    memset(&findData, 0, sizeof(findData));
    HANDLE find = FindFirstFileW(path->c_str(), &findData);
    if (find == INVALID_HANDLE_VALUE)
        return false;
    path->removeFileName();
    *path << findData.cFileName;
    FindClose(find);
    return true;
}

int Application::loadFromFile(iStream* in, String* path, bool decrypt)
{
    return static_cast<aStream*>(in)->load(path->c_str());
}

int Application::saveToFile(Bytes* data, String* path, bool encrypt)
{
    return static_cast<aStream*>(data)->save(path->c_str());
}

Doc* Application::findDocument(const wchar_t* path)
{
    int count = m_documents.size();
    int i = 0;
    if (count <= 0)
        return 0;
    Doc* doc;
    while (true)
    {
        doc = m_documents.get(i);
        if (doc->isPath(path))
            break;
        if (++i >= count)
            return 0;
    }
    return doc;
}

void Application::addDocument(Doc* doc)
{
    if (!doc)
        return;
    m_documents.add(doc);
    HMENU windowMenu = GetSubMenu(m_hMenu, 2);
    if (!windowMenu)
        return;
    UINT id = 0xE201;
    MENUITEMINFOW mii;
    memset(&mii, 0, sizeof(mii));
    mii.cbSize = sizeof(MENUITEMINFOW);
    mii.fMask = MIIM_ID;
    if (GetMenuItemInfoW(windowMenu, 0xE200, FALSE, &mii))
    {
        DeleteMenu(windowMenu, 0xE200, MF_BYCOMMAND);
    }
    else
    {
        memset(&mii, 0, sizeof(mii));
        mii.cbSize = sizeof(MENUITEMINFOW);
        mii.fMask = MIIM_ID;
        if (GetMenuItemInfoW(windowMenu, 0xE201, FALSE, &mii))
        {
            do
            {
                ++id;
                memset(&mii, 0, sizeof(mii));
                mii.cbSize = sizeof(MENUITEMINFOW);
                mii.fMask = MIIM_ID;
            } while (GetMenuItemInfoW(windowMenu, id, FALSE, &mii));
        }
    }
    ULONG_PTR data = (ULONG_PTR)doc->m_hWnd;
    memset(&mii, 0, sizeof(mii));
    mii.cbSize = sizeof(MENUITEMINFOW);
    mii.fMask = MIIM_STATE | MIIM_ID | MIIM_STRING;
    mii.fType = 0;
    mii.wID = id;
    mii.fState = 0;
    mii.dwTypeData = (LPWSTR)doc->getName();
    if (data)
    {
        mii.fMask = MIIM_STATE | MIIM_ID | MIIM_STRING | MIIM_DATA;
        mii.dwItemData = data;
    }
    InsertMenuItemW(windowMenu, 0xFFFFFFFF, FALSE, &mii);
}

void Application::removeDocument(Doc* doc)
{
    if (!doc)
        return;
    m_documents.detachItem(doc);
    HMENU windowMenu = GetSubMenu(m_hMenu, 2);
    if (!windowMenu)
        return;
    deleteMenuItemByData(windowMenu, (ULONG_PTR)doc->m_hWnd);
    if (GetMenuItemCount(windowMenu) == 2)
    {
        MENUITEMINFOW mi;
        memset(&mi, 0, sizeof(mi));
        mi.cbSize = sizeof(MENUITEMINFOW);
        mi.fMask = MIIM_STATE | MIIM_ID | MIIM_STRING;
        mi.fType = 0;
        mi.wID = 0xE200;
        mi.fState = MF_GRAYED;
        mi.dwTypeData = (LPWSTR)L"none";
        InsertMenuItemW(windowMenu, 0xFFFFFFFF, FALSE, &mi);
    }
}

void Application::updateDocument(FileTree* node)
{
    if (!node)
        return;
    Doc* doc = findDocument(node->m_path.c_str());
    if (!doc)
        return;
    HMENU windowMenu = GetSubMenu(m_hMenu, 2);
    if (!windowMenu)
        return;
    String name(node->m_name);
    String caption = name + L" *";
    setMenuItemTextByData(windowMenu, (ULONG_PTR)doc->m_hWnd, (LPWSTR)caption.c_str());
}

bool Application::closeAll()
{
    if (m_documents.size() <= 0)
        return true;
    while (true)
    {
        Doc* doc = m_documents.get(0);
        if (!doc->close())
            break;
        if (m_documents.size() <= 0)
            return true;
    }
    return false;
}

bool Application::openFile(LPCWSTR path)
{
    return openDocument(0, path, false);
}

bool Application::reopenDocument(FileTree* node)
{
    return openDocument(node, node->m_path.c_str(), true);
}

bool Application::openDocument(FileTree* node, LPCWSTR path, bool reload)
{
    setStatusText(0, 0);
    if (!path)
        return false;
    Doc* existing = findDocument(path);
    if (existing)
    {
        if (IsWindowEnabled(existing->m_hWnd))
            SetTimer(existing->m_hWnd, 0x82, 50, 0);
        return false;
    }
    String fileName;
    fileName.append(PathFindFileNameW(path));
    String ext;
    ext.append(PathFindExtensionW(path));
    ext.toLowerInPlace();
    bool created = false;
    if (!node)
    {
        String folder(path);
        folder.removeFileName();
        node = new FileTree(0, PathFindFileNameW(path));
        node->m_path = path;
        created = true;
    }
    Doc* viewer;
    bool result;
    if (ext == L".txt" || ext == L".cfg" || ext == L".csv" || ext == L".ini" || ext == L".sql" || ext == L".xml")
    {
        viewer = new TextView(node, true);
    }
    else if (ext == L".bmp" || ext == L".dds" || ext == L".gif" || ext == L".jpg" || ext == L".png" || ext == L".tga")
    {
        viewer = new ImageView(node);
    }
    else if (ext == L".wav" || ext == L".mp3")
    {
        if (created && node)
            delete node;
        ShellExecuteW(0, L"open", path, 0, 0, SW_SHOWNORMAL);
        return true;
    }
    else
    {
        viewer = createView(&ext, node, reload);
    }
    if (viewer)
    {
        if (!reload)
            viewer->setOwnedData(node);
        result = showDocument(viewer, PathFindFileNameW(path));
    }
    else
    {
        if (created && node)
            delete node;
        setStatusText(L"The file \"%s\" is not supported.", (INT_PTR)PathFindFileNameW(path));
        result = false;
    }
    return result;
}

bool Application::showDocument(Doc* doc, LPCWSTR name)
{
    if (!doc)
        return false;
    SetCursor(hCursor);
    POINT pt;
    pt.x = 0;
    pt.y = 0;
    if (GetCursorPos(&pt))
        SetCursorPos(pt.x + 1, pt.y + 1);
    bool opened = doc->openWindow();
    SetCursor(g_arrowCursor);
    if (!opened)
    {
        String message;
        message << L"The file \"%s\" can not be loaded or processed";
        if (g_lastLoadError)
        {
            message << g_lastLoadError;
            g_lastLoadError = 0;
        }
        setStatusText(message.c_str(), (INT_PTR)name);
        return false;
    }
    addDocument(doc);
    doc->show(true);
    SetTimer(doc->m_hWnd, 0x82, 50, 0);
    return true;
}

INT_PTR CALLBACK Application_aboutDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    INT_PTR result;
    switch (msg)
    {
    case WM_INITDIALOG:
        result = 1;
        break;
    case WM_COMMAND:
        if ((WORD)wParam != IDOK && (WORD)wParam != IDCANCEL)
            return 0;
        EndDialog(hDlg, (WORD)wParam);
        result = 1;
        break;
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORBTN:
    case WM_CTLCOLORDLG:
    case WM_CTLCOLORSTATIC:
        result = (INT_PTR)g_aboutBrush;
        if (!g_aboutBrush)
        {
            g_aboutBrush = CreateSolidBrush(0xFFFFFF);
            UnrealizeObject(g_aboutBrush);
            result = (INT_PTR)g_aboutBrush;
        }
        break;
    default:
        result = 0;
        break;
    }
    return result;
}

void Application::onDestroy()
{
    Preferences::saveWindowPlacement(m_hWnd);
    if (g_gdiplusToken)
    {
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }
    PostQuitMessage(0);
}

void Application::onActivate(bool active)
{
    if (active)
        SetTimer(m_hWnd, 0x81, 50, 0);
}

bool Application::onClose()
{
    return closeAll();
}

bool Application::onCommand(UINT code, HWND ctrl, UINT id)
{
    UINT c = code;
    if (code == 1 && !ctrl)
    {
        HWND foreground = GetForegroundWindow();
        Popup* popup = Popup::findDocPopup(foreground);
        if (popup && popup != this)
        {
            SendMessageW(foreground, WM_COMMAND, (WORD)id | 0x10000, 0);
            return true;
        }
        c = 1;
    }
    if ((c | 1) == 1 && !ctrl)
    {
        setStatusText(0, 0);
        switch (id)
        {
        case 0xE140:
            DialogBoxParamW(hModule, MAKEINTRESOURCEW(0x80), m_hWnd, Application_aboutDialogProc, 0);
            return true;
        case 0x12:
        case 0xE141:
            if (closeAll())
                DestroyWindow(m_hWnd);
            return true;
        case 0xE401:
            onOptions();
            return true;
        }
        if (id - 0xE201 <= 0xFD)
        {
            HMENU windowMenu = GetSubMenu(m_hMenu, 2);
            HWND target = (HWND)getMenuItemData(windowMenu, id);
            if (target)
            {
                WINDOWPLACEMENT placement;
                memset(&placement, 0, sizeof(placement));
                GetWindowPlacement(target, &placement);
                if (placement.showCmd == SW_SHOWMINIMIZED)
                    ShowWindow(target, SW_RESTORE);
                else
                    ShowWindow(target, SW_SHOW);
                SetTimer(target, 0x82, 50, 0);
                return true;
            }
            return true;
        }
    }
    if (id < 0xE100)
        return false;
    HWND child = GetDlgItem(m_hWnd, 101);
    if (!child)
        return false;
    SendMessageW(child, WM_COMMAND, (WORD)id | ((WORD)code << 16), (LPARAM)ctrl);
    return true;
}

bool Application::onDropFile(LPCWSTR path, HWND target, int targetId)
{
    return openFile(path);
}

void Application::onMenuSelect(HMENU menu, UINT item, UINT flags)
{
    if (!menu || flags == 0xFFFF)
        setStatusText(0, 0);
}

bool Application::onTimer(UINT_PTR id)
{
    if (id != 128)
        return id == 129;
    onStartup();
    return true;
}

LRESULT Application::defWindowProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_ACTIVATEAPP:
        if (!wParam)
        {
            if (g_currentModal)
                g_currentModal->reserved73();
        }
        m_active = wParam == 1;
        return DefWindowProcW(m_hWnd, msg, wParam, lParam);
    case WM_MENUSELECT:
        onMenuSelect((HMENU)lParam, LOWORD(wParam), HIWORD(wParam));
        return 0;
    case WM_DRAWCLIPBOARD:
        Clipboard_releaseGlobals();
        return 0;
    case 0x404:
        removeDocument((Doc*)lParam);
        return 0;
    case 0x405:
        if (lParam)
            updateDocument((FileTree*)lParam);
        return 0;
    }
    if (msg != 0x414)
        return DefWindowProcW(m_hWnd, msg, wParam, lParam);
    HANDLE mapping = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, L"FileShellParameters");
    if (!mapping)
        return 0;
    LPVOID view = MapViewOfFile(mapping, FILE_MAP_ALL_ACCESS, 0, 0, 0x104);
    if (view)
    {
        openFile((LPCWSTR)view);
        UnmapViewOfFile(view);
    }
    CloseHandle(mapping);
    return 0;
}
