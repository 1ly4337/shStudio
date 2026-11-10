#include "Doc.h"
#include "FileTree.h"
#include "Serializable.h"
#include "ContentProvider.h"
#include "MenuBitmaps.h"
#include "p10_vcall.h"
#include "AlertDial.h"
#include "ChangeCodepageDialog.h"
#include "FileDialogs.h"
#include "Application.h"
#include "_imports_p10.h"
#include <shlwapi.h>

static inline String& ftPath(FileTree* f)     { return FIELD(String, f, 0x10); }
static inline String& ftName(FileTree* f)     { return FIELD(String, f, 0x30); }
static inline int&    ftCodepage(FileTree* f) { return FIELD(int, f, 0x50); }
static inline int&    ftState(FileTree* f)    { return FIELD(int, f, 0xFC); }
#define FT_FLAGS(f)       P10_VCALL((f), 0x08, int)((f))
#define FT_TOUCH(f, tag)  P10_VCALL((f), 0x18, void, DWORD)((f), (tag))
#define FRAME_NOTIFY(o, v) P10_VCALL((o), 0x158, void, int, int)((o), (v), 0)

Doc::~Doc()
{
    if (m_object)
        delete static_cast<Serializable*>(m_object);
    if (m_ownedData)
        delete m_ownedData;
    if (m_hMenu)
        DestroyMenu(m_hMenu);
}

bool Doc::openWindow()
{
    RECT rc;
    SetRectEmpty(&rc);
    GetWindowRect(g_hMainWnd, &rc);
    rc.top += 50;
    rc.left += 60;
    rc.bottom = rc.top + 480;
    rc.right = rc.left + 700;
    SIZE def;
    def.cx = rc.right - rc.left;
    def.cy = rc.bottom - rc.top;
    SIZE size;
    SIZE* s = getDefaultSize(&size, def);
    rc.right = s->cx + rc.left;
    rc.bottom = s->cy + rc.top;
    String title;
    if (m_data)
    {
        String* src = ftPath(m_data).m_length ? &ftPath(m_data) : &ftName(m_data);
        title.append(src->m_buffer, src->m_length);
    }
    else
    {
        title = L"noname";
    }
    return createFrame(&rc, (WORD)getMenuId(), title.m_buffer);
}

bool Doc::close()
{
    bool result = onClose();
    if (result)
    {
        DestroyWindow(m_hWnd);
        return true;
    }
    return result;
}

void Doc::setModified(bool modified)
{
    if (modified)
    {
        if (!m_modified)
        {
            FileTree* data = m_data;
            m_modified = true;
            if (data)
            {
                ftState(data) = 2;
                SendMessageW(g_hMainWnd, WM_USER + 5, 0, (LPARAM)m_data);
                String* src = ftPath(m_data).m_length ? &ftPath(m_data) : &ftName(m_data);
                String title = *src + L" *";
                SetWindowTextW(m_hWnd, title.m_buffer);
            }
        }
    }
    else if (m_modified)
    {
        FileTree* data = m_data;
        m_modified = false;
        if (data)
        {
            ftState(data) = 1;
            SendMessageW(g_hMainWnd, WM_USER + 5, 0, (LPARAM)m_data);
            if (ftPath(m_data).m_length)
                SetWindowTextW(m_hWnd, ftPath(m_data).m_buffer);
            else
                SetWindowTextW(m_hWnd, ftName(m_data).m_buffer);
        }
    }
}

void Doc::setOwnedData(FileTree* data)
{
    m_ownedData = data;
}

bool Doc::onCreate()
{
    bool result = Popup::onCreate();
    if (result)
    {
        if (m_data && (FT_FLAGS(m_data) & 2) != 0)
            setModified(true);
        return true;
    }
    return result;
}

bool Doc::onClose()
{
    if (m_modified)
    {
        String text(L"Do you want to save the changes made to:");
        FileTree* data = m_data;
        (text << L"\r\n").append(ftPath(data).m_buffer, ftPath(data).m_length);
        int answer = AlertDial::alert(m_hWnd, text.m_buffer, 0, MB_ICONQUESTION | MB_YESNOCANCEL);
        if (answer == IDCANCEL || (answer == IDYES && !save()))
            return false;
    }
    if (m_ownedData)
        ftState(m_data) = 0;
    return true;
}

bool Doc::loadData()
{
    setStatusText(L"Loading data...", 0);
    HRESULT hr = load();
    setStatusText(L"Ready", 0);
    if (hr && hr != 4306)
        return false;
    m_modified = true;
    setModified(false);
    return true;
}

bool Doc::save()
{
    HRESULT hr = store();
    if (!hr)
    {
        FT_TOUCH(m_data, 0x74696D65);
        setModified(true);
        setModified(false);
        return true;
    }
    String text;
    FileTree* data = m_data;
    (text << L"The application failed to save the document ").append(ftName(data).m_buffer, ftName(data).m_length) << L".\n";
    if (hr == 0x13)
    {
        text << L"The data.saf file is opened by the client game and can not be updated.\n";
        text << L"Terminate the game prior to try again to save this file.";
    }
    else
    {
        text.m_radix = 16;
        (text << L"The error was: 0x") << (long)hr;
        text.m_radix = 10;
        (text << L" (") << (long)hr << L")";
    }
    AlertDial::alert(m_hWnd, text.m_buffer, L"Error", MB_ICONHAND);
    return false;
}

bool Doc::saveAs()
{
    bool result;
    String path(ftPath(m_data));
    if (g_contentProvider && g_contentProvider->isArchive())
        path = g_contentProvider->getDirectory();
    path.removeFileName();
    wchar_t* ext = PathFindExtensionW(ftName(m_data).m_buffer);
    if (ext)
        ++ext;
    int count = 0;
    DWORD filterIndex = (DWORD)getFilterIndex();
    COMDLG_FILTERSPEC* filters = getFilters(&count);
    if (FileDialogs_save(m_hWnd, &path, ext, filters, count, &filterIndex, 0))
    {
        result = false;
    }
    else
    {
        String oldName(ftName(m_data));
        String oldPath(ftPath(m_data));
        if (g_contentProvider && g_contentProvider->isArchive() && !m_ownedData)
        {
            FileTree* previous = m_data;
            FileTree* copy = new FileTree();
            m_ownedData = copy;
            m_data = copy;
            ftPath(copy) = path;
            ftName(m_data) = PathFindFileNameW(path.m_buffer);
            if (save())
            {
                result = true;
            }
            else
            {
                if (m_ownedData)
                    delete m_ownedData;
                m_data = previous;
                m_ownedData = 0;
                result = false;
            }
        }
        else
        {
            int oldFilterIndex = getFilterIndex();
            FileTree* data = m_data;
            int oldCodepage = ftCodepage(data);
            ftPath(data) = path;
            ftName(m_data) = PathFindFileNameW(path.m_buffer);
            ftCodepage(m_data) = 'file';
            setFilterIndex((int)filterIndex);
            result = save();
            ftPath(m_data) = oldPath;
            ftName(m_data) = oldName;
            ftCodepage(m_data) = oldCodepage;
            setFilterIndex(oldFilterIndex);
        }
    }
    return result;
}

bool Doc::onTimer(UINT_PTR id)
{
    if (id != 130)
        return false;
    SetForegroundWindow(m_hWnd);
    onActivated();
    return true;
}

bool Doc::onCommand(UINT code, HWND ctrl, UINT id)
{
    if ((code | 1) == 1 && !ctrl)
    {
        if (id == 0xE102)
        {
            if (onClose())
                DestroyWindow(m_hWnd);
            return true;
        }
        if (id == 0xE103)
        {
            save();
            return true;
        }
        if (id == 0xE435)
        {
            if (!m_data)
                return true;
            int codepage = ChangeCodepageDialog::run(m_hWnd, reinterpret_cast<Document*>(this));
            if (codepage < 0)
                return true;
            FileTree* data = m_data;
            if (codepage == ftCodepage(data))
                return true;
            ftCodepage(data) = codepage;
            load();
            invalidate(true);
            return true;
        }
        if (id - 0xE100u <= 0x1F)
            return true;
    }
    if (id < 0xE100)
        return false;
    HWND child = GetDlgItem(m_hWnd, 101);
    if (!child)
        return false;
    SendMessageW(child, WM_COMMAND, (WORD)id | ((WORD)code << 16), (LPARAM)ctrl);
    return true;
}

bool Doc::onKey(WPARAM vk, LPARAM flags, bool down)
{
    if (GetKeyState(VK_CONTROL) >= 0 || (short)vk != 'W')
        return false;
    onCommand(0, 0, 0xE102);
    return true;
}

void Doc::updateMenu(HMENU menu)
{
    EnableMenuItem(menu, 0xE102u, MF_ENABLED);
    EnableMenuItem(menu, 0xE103u, MF_ENABLED);
    EnableMenuItem(menu, 0xE128u, MF_ENABLED);
    EnableMenuItem(menu, 0xE129u, MF_ENABLED);
    setMenuItemBitmap(menu, 0xE120u);
    setMenuItemBitmap(menu, 0xE121u);
    setMenuItemBitmap(menu, 0xE122u);
    setMenuItemBitmap(menu, 0xE123u);
    setMenuItemBitmap(menu, 0xE124u);
    setMenuItemBitmap(menu, 0xE125u);
    setMenuItemBitmap(menu, 0xE128u);
    setMenuItemBitmap(menu, 0xE12Au);
    setMenuItemBitmap(menu, 0xE12Bu);
    setMenuItemBitmap(menu, 0xE12Cu);
    setMenuItemBitmap(menu, 0xE102u);
    setMenuItemBitmap(menu, 0xE103u);
    setMenuItemBitmap(menu, 0xE11Eu);
    setMenuItemBitmap(menu, 0xE105u);
    updateScriptMenu(menu);
}

bool Doc::enableMenuItem(UINT id, bool enable)
{
    HMENU menu = m_hMenu;
    return menu && EnableMenuItem(menu, id, enable == 0) != -1;
}

LPCWSTR Doc::getCommandHelp(UINT id)
{
    if (id == 58416)
        return L"Import text fields from a language file";
    if (id == 58417)
        return L"Import definition from database";
    if (id != 58421)
        return 0;
    return L"Change codepage...";
}

LRESULT Doc::setState(int state)
{
    ftState(m_data) = state;
    return SendMessageW(g_hMainWnd, WM_USER + 5, 0, (LPARAM)m_data);
}

int Doc::notifyFrame(int value)
{
    FRAME_NOTIFY(g_mainFrame, value);
    return value;
}

String Doc::getDirectory()
{
    String result(getPath());
    result.removeFileName();
    return result;
}

LRESULT Doc::defWindowProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg != WM_USER + 3)
        return DefWindowProcW(m_hWnd, msg, wParam, lParam);
    setModified(wParam != 0);
    return 0;
}
