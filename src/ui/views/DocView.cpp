#include "DocView.h"
#include "SearchPanel.h"
#include "Serializable.h"
#include "Bytes.h"
#include "aStream.h"
#include "FileTree.h"
#include "ContentProvider.h"
#include "ListCtrl.h"
#include "ListCtrlOD.h"
#include "TabControl.h"
#include "Control.hpp"
#include "Displayable.h"
#include "TextSearch.h"
#include "CountryNames.h"
#include "p10_vcall.h"
#include "AlertDial.h"
#include "ChooseCodepageDialog.h"
#include "Application.h"
#include "_imports_p10.h"
#include <shlwapi.h>
#include <commctrl.h>

#define DV_SEARCH(d)   (&(d)->m_search)
#define DV_EDIT(d)     FIELD(HWND, &(d)->m_search, 0x38)
#define DV_STATUS(d)   FIELD(HWND, &(d)->m_search, 0x3C)
#define DV_CONTENT(d)  ((d)->m_content)
#define DV_MARGIN(d)   ((d)->m_margin)
#define DV_SEARCHON(d) ((d)->m_searchEnabled)
#define DV_DECRYPT(d)  ((d)->m_decrypt)

static inline String& ftPath(FileTree* f)     { return FIELD(String, f, 0x10); }
static inline String& ftName(FileTree* f)     { return FIELD(String, f, 0x30); }
static inline int&    ftCodepage(FileTree* f) { return FIELD(int, f, 0x50); }
static inline int&    ftState(FileTree* f)    { return FIELD(int, f, 0xFC); }
static inline int&    ftType(FileTree* f)     { return FIELD(int, f, 0x140); }
#define FT_FLAGS(f) P10_VCALL((f), 0x08, int)((f))

#define APP_EXISTS(a, s)        P10_VCALL((a), 0x148, bool, const String*)((a), (s))
#define APP_READ(a, b, s, dec)  P10_VCALL((a), 0x14C, HRESULT, Bytes*, const String*, bool)((a), (b), (s), (dec))
#define APP_WRITE(a, b, s, enc) P10_VCALL((a), 0x150, HRESULT, Bytes*, const String*, bool)((a), (b), (s), (enc))

static const DWORD kFmtPlain = 'file';
static const DWORD kFmtTrans = 'flnt';
static const DWORD kFmtLang  = 'ltxt';

#define DV_CONTENT_RECT(d, rc) ((d)->getContentRect(rc))
#define DV_FIND_NEXT(d)        ((d)->findNext())
#define DV_HAS_TRANSLATION(d)  ((d)->hasTranslation())
#define DV_DETECT_CODEPAGE(d)  ((d)->detectsCodepage())

static void dvLayoutChild(DocView* view)
{
    HWND child = GetDlgItem(view->m_hWnd, 101);
    RECT rc;
    DV_CONTENT_RECT(view, &rc);
    SetWindowPos(child, 0, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, 6u);
}

RECT* DocView::getClientRect(RECT* out)
{
    RECT rc;
    GetClientRect(m_hWnd, &rc);
    rc.bottom -= m_statusHeight;
    CopyRect(out, &rc);
    if (DV_SEARCH(this)->isVisible())
        out->bottom -= 26;
    return out;
}

RECT* DocView::getContentRect(RECT* out)
{
    getClientRect(out);
    int margin = DV_MARGIN(this);
    out->top += margin;
    out->bottom -= margin;
    out->left += margin;
    out->right -= margin;
    return out;
}

bool DocView::onCreate()
{
    if (!Popup::onCreate())
        return false;
    if (m_data && (FT_FLAGS(m_data) & 2) != 0)
        setModified(true);
    RECT rc;
    getClientRect(&rc);
    rc.top = rc.bottom - 26;
    DV_SEARCH(this)->Window::create(m_hWnd, (HMENU)0x3F3, &rc, WS_CHILD, 0, 0);
    if ((GetKeyState(VK_LCONTROL) & 0x8080) != 0)
        DV_CONTENT(this)->clear();
    if (DV_CONTENT(this)->isEmpty())
    {
        if (!loadData())
            return false;
    }
    else
    {
        int flags = FT_FLAGS(m_data);
        if (flags == 2)
        {
            setModified(true);
            ftState(m_data) = 1;
        }
        else if (flags != 1)
        {
            ftState(m_data) = 1;
        }
    }
    show(true);
    return true;
}

bool DocView::onCommand(UINT code, HWND ctrl, UINT id)
{
    if ((code | 1) != 1 || ctrl)
        return Doc::onCommand(code, ctrl, id);
    if (id == 0xE129)
    {
        if (!DV_SEARCHON(this))
            return true;
        if (DV_SEARCH(this)->isVisible() || SendMessageW(DV_EDIT(this), WM_GETTEXTLENGTH, 0, 0))
        {
            DV_FIND_NEXT(this);
            return true;
        }
    }
    else if (id != 0xE128)
    {
        return Doc::onCommand(code, ctrl, id);
    }
    if (!DV_SEARCHON(this))
        return true;
    bool wasVisible = DV_SEARCH(this)->isVisible() != 0;
    DV_SEARCH(this)->show(true);
    if (wasVisible)
        return true;
    dvLayoutChild(this);
    return true;
}

LRESULT DocView::defWindowProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_USER + 2:
    {
        DV_SEARCH(this)->show(false);
        HWND child = GetDlgItem(m_hWnd, 101);
        RECT rc;
        DV_CONTENT_RECT(this, &rc);
        SetWindowPos(child, 0, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, 6u);
        SetFocus(child);
        return 0;
    }
    case WM_USER + 1:
        if (DV_SEARCHON(this))
            DV_FIND_NEXT(this);
        return 0;
    case WM_USER + 3:
        setModified(wParam != 0);
        return 0;
    default:
        return DefWindowProcW(m_hWnd, msg, wParam, lParam);
    }
}

bool DocView::onKey(WPARAM vk, LPARAM flags, bool down)
{
    if (down && (short)vk == VK_ESCAPE)
    {
        if (DV_SEARCH(this)->isVisible())
        {
            DV_SEARCH(this)->show(false);
            dvLayoutChild(this);
        }
        return true;
    }
    if (GetKeyState(VK_CONTROL) < 0 && (short)vk == 'W')
    {
        onCommand(0, 0, 0xE102);
        return true;
    }
    return false;
}

void DocView::onSize(WPARAM type, int cx, int cy)
{
    int bottom = cy;
    if (m_hStatusBar)
    {
        SetWindowPos(m_hStatusBar, 0, 0, cy - m_statusHeight, cx, m_statusHeight, SWP_NOZORDER);
        bottom = cy - m_statusHeight;
    }
    SetWindowPos(DV_SEARCH(this)->m_hWnd, 0, 0, bottom - 26, cx, 26, 0x24u);
    dvLayoutChild(this);
}

void DocView::fillScriptMenu(HMENU menu)
{
    appendMenuItem(menu, 0xE435u);
    appendMenuItem(menu, 0xE430u);
}

HRESULT DocView::load()
{
    void* app = g_studioFrame;
    if (!app)
        return E_POINTER;
    if (!m_data)
        return 4306;
    Bytes bytes(0, 0u);
    HRESULT result = APP_READ(app, &bytes, &ftPath(m_data), DV_DECRYPT(this));
    if (result)
        return result;
    try
    {
    String path(ftPath(m_data));
    path.removeFileName();
    String fileName(PathFindFileNameW(ftPath(m_data).m_buffer), (unsigned)-1);
    DWORD format;
    if (fileName.startsWith(L"DB"))
    {
        format = kFmtTrans;
        path.append(fileName.m_buffer, fileName.m_length);
        path.removeExtension();
        for (int i = 0; i < 4; ++i)
        {
            if (!path.m_length)
                break;
            path.m_buffer[--path.m_length] = 0;
        }
        path << L"Text_";
    }
    else if (fileName.startsWith(L"NpcQuest"))
    {
        path.append(fileName.m_buffer, fileName.m_length);
        path.removeExtension();
        path << L"Trans_";
        bool found;
        {
            String pattern = path + L"*.sdata";
            found = APP_EXISTS(g_studioFrame, &pattern);
        }
        format = kFmtPlain;
        if (found)
            format = kFmtTrans;
    }
    else
    {
        format = kFmtPlain;
        path.clear();
    }
    int language = (int)g_preferences.getInt(L"SData", L"language", 840);
    if (format != kFmtPlain && DV_HAS_TRANSLATION(this))
    {
        String languageFile(path);
        const wchar_t* suffix;
        if (fileName.startsWith(L"NpcQuest"))
            suffix = countryName(language);
        else
            suffix = countryCode(language);
        (languageFile << suffix) << L".SData";
        bool exists;
        if (APP_EXISTS(g_studioFrame, &ftPath(m_data)))
            exists = APP_EXISTS(g_studioFrame, &languageFile);
        else
            exists = PathFileExistsW(languageFile.m_buffer) != 0;
        if (!exists)
        {
            String text(L"The file ", (unsigned)-1);
            ((text << PathFindFileNameW(languageFile.m_buffer)) << L" which is required to load the file ")
                .append(fileName.m_buffer, fileName.m_length)
                << L" does not exist. You must choose another language in the settings or have that file in the "
                   L"repository and reopen it to finalize the action.";
            AlertDial::alert(g_hMainWnd, text.m_buffer, L"Error", MB_ICONHAND);
            return E_FAIL;
        }
        path = languageFile;
    }
    ftCodepage(m_data) = (int)format;
    int codepage = ftType(m_data);
    if (DV_DETECT_CODEPAGE(this))
    {
        bool detect = false;
        if (!codepage)
        {
            if (format == kFmtTrans)
            {
                if (language <= 410)
                {
                    if (language == 410)
                        codepage = 949;
                    else if (language == 156)
                        codepage = 936;
                    else if (language == 250 || language == 276 || language == 380)
                        codepage = 1252;
                    else
                        detect = true;
                }
                else if (language == 616 || language == 792)
                    codepage = 1250;
                else if (language == 840)
                    codepage = 1252;
                else
                    detect = true;
            }
            else
            {
                detect = true;
            }
            if (detect)
            {
                codepage = ChooseCodepageDialog::run(FIELD(HWND, g_mainFrame, 0x14), ftName(m_data).m_buffer);
                if (codepage <= 0)
                    return E_ABORT;
            }
        }
        else if (codepage < 0)
        {
            return E_ABORT;
        }
        static_cast<aStream&>(bytes).m_codePage = (UINT)codepage;
        ftType(m_data) = codepage;
    }
    Serializable* content = DV_CONTENT(this);
    content->clear();
    static_cast<Streamable*>(DV_CONTENT(this))->read(bytes, format);
    if (format == kFmtTrans && DV_HAS_TRANSLATION(this))
    {
        updateScriptMenu(m_hMenu);
        result = APP_READ(g_studioFrame, &bytes, &path, false);
        if (result)
        {
            String text(L"The file ", (unsigned)-1);
            text.append(path.m_buffer, path.m_length) << L" cannot be read.\nVerify it is not locked by another process and try again.";
            AlertDial::alert(g_hMainWnd, text.m_buffer, L"Error", MB_ICONHAND);
        }
        else
        {
            try
            {
                DV_CONTENT(this)->load(bytes, 0, language);
                DV_CONTENT(this)->setFileName(path.m_buffer);
            }
            catch (wchar_t* message)
            {
                size_t count = wcslen(message) + wcslen(path.m_buffer) + 0x40;
                wchar_t* text = new wchar_t[count];
                swprintf_s(text, count, message, path.m_buffer);
                AlertDial::alert(g_hMainWnd, text, L"Error", MB_ICONHAND);
                delete[] text;
                result = E_FAIL;
            }
            catch (...)
            {
                result = E_FAIL;
            }
        }
    }
    }
    catch (...)
    {
        DV_CONTENT(this)->clear();
        result = E_FAIL;
    }
    return result;
}

HRESULT DocView::store()
{
    if (!g_studioFrame || !m_data)
        return E_POINTER;
    HRESULT result;
    try
    {
    int codepage;
    if (!g_contentProvider || (codepage = FIELD(int, g_contentProvider, 8)) <= 0)
    {
        codepage = ftType(m_data);
        if (codepage <= 0)
        {
            codepage = DV_CONTENT(this)->getCodePage();
            if (codepage <= 0)
                codepage = ChooseCodepageDialog::run(m_hWnd, ftName(m_data).m_buffer);
        }
    }
    Bytes bytes(0, (UINT)codepage);
    static_cast<Streamable*>(DV_CONTENT(this))->write(bytes, (DWORD)ftCodepage(m_data));
    result = APP_WRITE(g_studioFrame, &bytes, &ftPath(m_data), DV_DECRYPT(this));
    if (!result && ftCodepage(m_data) == (int)kFmtTrans)
    {
        const wchar_t* languageFile = DV_CONTENT(this)->m_fileName;
        if (languageFile)
        {
            static_cast<aStream&>(bytes).wipe();
            static_cast<Streamable*>(DV_CONTENT(this))->write(bytes, kFmtLang);
            String target(languageFile, (unsigned)-1);
            result = APP_WRITE(g_studioFrame, &bytes, &target, false);
        }
    }
    }
    catch (wchar_t* message)
    {
        AlertDial::alert(g_hMainWnd, message, L"Error", MB_ICONHAND);
        result = E_FAIL;
    }
    catch (...)
    {
        result = E_FAIL;
    }
    return result;
}

void DocView::findNext()
{
    SendMessageW(DV_STATUS(this), WM_SETTEXT, 0, (LPARAM)L"");
    HWND child = GetDlgItem(m_hWnd, 101);
    Control* control;
    if (child)
    {
        DWORD_PTR ref = 0;
        control = GetWindowSubclass(child, Control::subclassProc, 0x6374726Cu , &ref) ? (Control*)ref : 0;
    }
    else
    {
        control = 0;
    }
    if (dynamic_cast<TabControl*>(control))
    {
        HWND page = dynamic_cast<TabControl*>(control)->getPage(-1);
        if (page)
        {
            DWORD_PTR ref = 0;
            control = GetWindowSubclass(page, Control::subclassProc, 0x6374726Cu, &ref) ? (Control*)ref : 0;
        }
        else
        {
            control = 0;
        }
    }
    ListCtrl* list = dynamic_cast<ListCtrl*>(control);
    bool useNodes = true;
    if (!list)
        return;
    int column = list->listV69();
    if (!column)
        return;
    if (!dynamic_cast<ListCtrlOD*>(control))
    {
        int count = FIELD(int, list, 0x24);
        int i = 0;
        useNodes = false;
        if (count <= 0)
            return;
        for (;;)
        {
            long id = (i < 0 || i >= count) ? 0 : FIELD(long*, list, 0x20)[i];
            if (id == column)
                break;
            if (++i >= count)
                return;
        }
        column = i;
        if (i < 0)
            return;
    }
    int start = list->getNextSelected(0);
    if (start < 0)
        start = 0;
    SearchPanel* search = DV_SEARCH(this);
    String text = search->getText();
    if (text.m_length >= 3)
    {
        const wchar_t* needle = text.m_buffer;
        int count = list->getCount(0);
        int i = start;
        for (;;)
        {
            if (++i >= count)
                i = 0;
            if (i == start)
                break;
            if (useNodes)
            {
                Node* node = list->getNode(i);
                const wchar_t* s = node ? node->getString((DWORD)column) : 0;
                if (findNoCase(s, needle))
                {
                    list->selectItem(i);
                    return;
                }
            }
            else
            {
                String cell = list->getCellText(i, column);
                if (cell.m_length && findNoCase(cell.m_buffer, needle))
                {
                    list->selectItem(i);
                    return;
                }
            }
        }
        search->setStatus(L"no match found");
    }
}
