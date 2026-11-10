#include "FileDialogs.h"
#include "ComDlgFiltersSpec.h"
#include "String.hpp"
#include "Window.h"
#include <commdlg.h>
#include <shlobj.h>
#include <shlwapi.h>
#include "_imports_p10.h"

HRESULT (WINAPI* g_SHCreateItemFromParsingName)(PCWSTR, IBindCtx*, REFIID, void**) = 0;
DWORD g_shellItemApiError = 0;
BrowseFolderParams g_browseFolderParams = { 0, 0, 0 };

bool FileDialogs_shellItemApiMissing()
{
    HRESULT (WINAPI* fn)(PCWSTR, IBindCtx*, REFIID, void**) = g_SHCreateItemFromParsingName;
    if (!g_SHCreateItemFromParsingName)
    {
        if (!g_shell32.m_module)
        {
            g_shellItemApiError = 2;
            return g_SHCreateItemFromParsingName == 0;
        }
        fn = (HRESULT (WINAPI*)(PCWSTR, IBindCtx*, REFIID, void**))GetProcAddress(g_shell32.m_module, "SHCreateItemFromParsingName");
        if (!fn)
        {
            g_shellItemApiError = GetLastError();
            g_SHCreateItemFromParsingName = 0;
            return true;
        }
        g_shellItemApiError = 0;
        g_SHCreateItemFromParsingName = fn;
    }
    return fn == 0;
}

bool FileDialogs_openLegacy(HWND owner, String* path, const wchar_t* filter, const wchar_t* title, bool multiSelect)
{
    WCHAR file[262];
    memset(file, 0, 0x208);
    if (!owner)
        owner = GetActiveWindow();
    unsigned len = path->m_length;
    if (len && len < 260 && path->m_buffer[len - 1] != L'\\' && PathFileExistsW(path->m_buffer))
        memcpy(file, path->m_buffer, path->m_length);
    OPENFILENAMEW ofn;
    memset(&ofn, 0, sizeof(ofn));
    unsigned len2 = path->m_length;
    ofn.lpstrFile = file;
    ofn.lpstrTitle = title;
    ofn.lStructSize = 88;
    ofn.hwndOwner = owner;
    ofn.nMaxFile = 260;
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.Flags = multiSelect ? 0x21004 : 0x1004;
    if (len2)
    {
        const wchar_t* initialDir = ofn.lpstrInitialDir;
        if (path->m_buffer[len2 - 1] == L'\\')
            initialDir = path->m_buffer;
        ofn.lpstrInitialDir = initialDir;
    }
    if (GetOpenFileNameW(&ofn))
    {
        *path = file;
        return true;
    }
    CommDlgExtendedError();
    return false;
}

int FileDialogs_saveLegacy(HWND owner, String* path, const wchar_t* filter, DWORD* filterIndex, const wchar_t* title)
{
    wchar_t file[262];
    memset(file, 0, 0x208);
    unsigned len = path->m_length;
    if (len && len < 260)
        wcscpy_s(file, 260, path->m_buffer);
    OPENFILENAMEW ofn;
    memset(&ofn, 0, sizeof(ofn));
    ofn.lpstrFile = file;
    ofn.lpstrTitle = title;
    ofn.lpstrInitialDir = path->m_buffer;
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = *filterIndex;
    ofn.lStructSize = 88;
    ofn.hwndOwner = owner;
    ofn.nMaxFile = 260;
    ofn.Flags = 0x806;
    ofn.lpstrDefExt = NULL;
    if (!GetSaveFileNameW(&ofn))
        return ERROR_CANCELLED;
    *path = file;
    *filterIndex = ofn.nFilterIndex;
    return 0;
}

int CALLBACK FileDialogs_browseCallback(HWND hWnd, UINT msg, LPARAM lParam, LPARAM lpData)
{
    BrowseFolderParams* params = (BrowseFolderParams*)lpData;
    if (params)
    {
        if (msg == BFFM_INITIALIZED)
        {
            SetWindowLongW(hWnd, GWL_STYLE, GetWindowLongW(hWnd, GWL_STYLE) & 0xFFFFDFFF);
            SetWindowLongW(hWnd, GWL_EXSTYLE, GetWindowLongW(hWnd, GWL_EXSTYLE) & 0xFFFFFBFF);
            ShowWindow(GetDlgItem(hWnd, 14146), SW_HIDE);
            HWND tree = FindWindowExW(hWnd, NULL, L"SysTreeView32", NULL);
            if (tree || (tree = FindWindowExW(hWnd, NULL, L"SHBrowseForFolder ShellNameSpace Control", NULL)) != NULL)
            {
                RECT rc;
                GetWindowRect(tree, &rc);
                rc.top -= 48;
                rc.left -= 8;
                rc.right += 8;
                rc.bottom += 8;
                ScreenToClient(hWnd, (LPPOINT)&rc);
                ScreenToClient(hWnd, (LPPOINT)&rc.right);
                MoveWindow(tree, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);
            }
            SetWindowTextW(hWnd, params->title);
            SendMessageW(hWnd, BFFM_SETSELECTIONW, 1, (LPARAM)params->initialPath);
            HWND owner = GetWindow(hWnd, GW_OWNER);
            if (owner)
                Window_centerOn(hWnd, owner);
        }
        else if (msg == BFFM_SELCHANGED)
        {
            if (params->validator)
            {
                WCHAR path[262];
                memset(path, 0, 0x104);
                if (SHGetPathFromIDListW((LPCITEMIDLIST)lParam, path))
                {
                    if (!params->validator(path))
                        SendMessageW(hWnd, BFFM_ENABLEOK, 0, 0);
                }
            }
        }
    }
    return 0;
}

bool FileDialogs_browseFolderLegacy(HWND owner, String* path, const wchar_t* title, int unused, bool newStyle,
                                    FolderValidator validator)
{
    g_browseFolderParams.title = title;
    g_browseFolderParams.initialPath = path->m_buffer;
    g_browseFolderParams.validator = validator;
    WCHAR display[260];
    memset(display, 0, 0x104);
    LPITEMIDLIST root = NULL;
    SHGetSpecialFolderLocation(owner, CSIDL_DESKTOP, &root);
    BROWSEINFOW bi;
    bi.hwndOwner = NULL;
    bi.pidlRoot = root;
    bi.lpszTitle = NULL;
    bi.pszDisplayName = display;
    bi.ulFlags = newStyle ? 581 : 69;
    bi.iImage = 0;
    bi.hwndOwner = owner;
    bi.lpszTitle = title;
    bi.lpfn = FileDialogs_browseCallback;
    bi.lParam = (LPARAM)&g_browseFolderParams;
    LPITEMIDLIST selected = SHBrowseForFolderW(&bi);
    bool ok = selected != NULL;
    if (selected)
    {
        BOOL got = SHGetPathFromIDListW(selected, display);
        ok = got != 0;
        if (got)
        {
            *path = display;
            String lower;
            lower.append(display, (unsigned)-1);
            lower.toLowerInPlace();
            if (lower.endsWithNoCase(L".zip"))
                ok = false;
            else if (!lower.m_length || lower.m_buffer[lower.m_length - 1] != L'\\')
                *path << '\\';
        }
    }
    CoTaskMemFree(root);
    CoTaskMemFree(selected);
    return ok;
}

bool FileDialogs_getItemPath(IShellItem* item, String* path)
{
    path->m_length = 0;
    path->m_capacity = 0;
    if (path->m_buffer)
    {
        operator delete[](path->m_buffer);
        path->m_buffer = NULL;
    }
    if (!item)
        return false;
    LPWSTR name = NULL;
    item->GetDisplayName(SIGDN_FILESYSPATH, &name);
    item->Release();
    if (!name)
        return false;
    *path = name;
    CoTaskMemFree(name);
    return true;
}

bool FileDialogs_selectFolder(HWND owner, String* path, const wchar_t* title, int unused, bool newStyle,
                              FolderValidator validator)
{
    if (CoInitializeEx(NULL, COINIT_APARTMENTTHREADED) < 0)
        return false;
    bool ok = false;
    IFileOpenDialog* dlg = NULL;
    CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_IFileOpenDialog, (void**)&dlg);
    if (FileDialogs_shellItemApiMissing())
    {
        if (dlg)
            dlg->Release();
        CoUninitialize();
        return FileDialogs_browseFolderLegacy(owner, path, title, unused, newStyle, validator);
    }
    if (!dlg)
    {
        CoUninitialize();
        return FileDialogs_browseFolderLegacy(owner, path, title, unused, newStyle, validator);
    }
    if (title)
        dlg->SetTitle(title);
    FILEOPENDIALOGOPTIONS options = 0;
    dlg->GetOptions(&options);
    dlg->SetOptions(options | FOS_PICKFOLDERS);
    SelectFolderFilter filter(validator);
    DWORD cookie = 0;
    if (validator)
        dlg->Advise(&filter, &cookie);
    IShellItem* item = NULL;
    g_SHCreateItemFromParsingName(path->m_buffer, NULL, IID_IShellItem, (void**)&item);
    if (item)
    {
        dlg->SetFolder(item);
        item->Release();
    }
    if (dlg->Show(NULL) >= 0 && dlg->GetResult(&item) >= 0)
    {
        ok = FileDialogs_getItemPath(item, path);
        if (ok)
            *path << '\\';
    }
    if (validator)
        dlg->Unadvise(cookie);
    dlg->Release();
    CoUninitialize();
    return ok;
}

bool FileDialogs_open(HWND owner, String* path, const wchar_t* filter, const wchar_t* title, bool multiSelect)
{
    if (CoInitializeEx(NULL, COINIT_APARTMENTTHREADED) < 0)
        return false;
    IFileOpenDialog* dlg = NULL;
    CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_IFileOpenDialog, (void**)&dlg);
    bool missing = FileDialogs_shellItemApiMissing();
    if (missing)
    {
        if (dlg)
            dlg->Release();
    }
    else if (dlg)
    {
        if (title)
            dlg->SetTitle(title);
        FILEOPENDIALOGOPTIONS options = 0;
        dlg->GetOptions(&options);
        options = (options & 0xFFFFEDFF) | FOS_FILEMUSTEXIST;
        dlg->SetOptions(options);

        IShellItem* folder;
        bool folderSet = false;
        if (path->m_length)
        {
            if (!path->endsWithNoCase(L"\\"))
            {
                dlg->SetFileName(PathFindFileNameW(path->m_buffer));
                path->removeFileName();
            }
            if (path->m_length && path->endsWithNoCase(L"\\"))
            {
                folder = NULL;
                g_SHCreateItemFromParsingName(path->m_buffer, NULL, IID_IShellItem, (void**)&folder);
                if (folder)
                {
                    dlg->SetFolder(folder);
                    folder->Release();
                    folderSet = true;
                }
            }
        }
        if (!folderSet)
        {
            folder = NULL;
            if (dlg->GetFolder(&folder) == S_OK)
            {
                LPWSTR name = NULL;
                if (folder->GetDisplayName(SIGDN_FILESYSPATH, &name) >= 0)
                {
                    *path = name;
                    CoTaskMemFree(name);
                }
                folder->Release();
                folder = NULL;
                String current(*path);
                if (current.endsWithNoCase(L"Desktop"))
                {
                    current = L"C:\\Users";
                    g_SHCreateItemFromParsingName(current.m_buffer, NULL, IID_IShellItem, (void**)&folder);
                    if (folder)
                    {
                        dlg->SetFolder(folder);
                        folder->Release();
                    }
                }
            }
        }

        ComDlgFiltersSpec specs;
        const wchar_t* p = filter;
        if (filter)
        {
            try
            {
                while (*p)
                {
                    const wchar_t* name = p;
                    const wchar_t* spec = &p[wcslen(p) + 1];
                    p = &spec[wcslen(spec) + 1];
                    if (!*name || !*spec)
                        throw 0;
                    _COMDLG_FILTERSPEC* entry = ComDlgFiltersSpec_newEntry(name, spec);
                    if (entry)
                        specs.add(entry);
                }
                if (specs.m_count > 0)
                    dlg->SetFileTypes(specs.m_count, specs.toArray());
            }
            catch (...)
            {
            }
        }

        HRESULT hr = dlg->Show(NULL);
        if (hr >= 0)
        {
            IShellItem* result = NULL;
            hr = dlg->GetResult(&result);
            if (hr >= 0)
            {
                LPWSTR name = NULL;
                hr = result->GetDisplayName(SIGDN_FILESYSPATH, &name);
                if (hr >= 0)
                {
                    *path = name;
                    CoTaskMemFree(name);
                }
                result->Release();
            }
        }
        dlg->Release();
        specs.deleteAll();
        CoUninitialize();
        return hr >= 0;
    }
    CoUninitialize();
    String titleCopy;
    titleCopy.append(title, (unsigned)-1);
    return FileDialogs_openLegacy(owner, path, filter, titleCopy.m_buffer, multiSelect);
}

unsigned FileDialogs_save(HWND owner, String* path, const wchar_t* defaultExt, const COMDLG_FILTERSPEC* specs,
                          int specCount, DWORD* typeIndex, const wchar_t* title)
{
    unsigned result = (unsigned)CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (result > 1)
        return result;
    IFileSaveDialog* dlg = NULL;
    CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_INPROC_SERVER, IID_IFileSaveDialog, (void**)&dlg);
    bool missing = FileDialogs_shellItemApiMissing();
    if (missing)
    {
        if (dlg)
            dlg->Release();
    }
    else if (dlg)
    {
        if (title)
            dlg->SetTitle(title);
        FILEOPENDIALOGOPTIONS options = 0;
        dlg->GetOptions(&options);
        if (options & FOS_ALLOWMULTISELECT)
        {
            options &= ~FOS_ALLOWMULTISELECT;
            dlg->SetOptions(options);
        }
        if (path->m_length)
        {
            if (!path->endsWithNoCase(L"\\"))
            {
                dlg->SetFileName(PathFindFileNameW(path->m_buffer));
                path->removeFileName();
            }
            if (path->m_length && path->endsWithNoCase(L"\\"))
            {
                IShellItem* folder = NULL;
                g_SHCreateItemFromParsingName(path->m_buffer, NULL, IID_IShellItem, (void**)&folder);
                if (folder)
                {
                    dlg->SetFolder(folder);
                    folder->Release();
                }
            }
        }
        if (defaultExt)
            dlg->SetDefaultExtension(defaultExt);
        if (specs)
        {
            dlg->SetFileTypes(specCount, specs);
            dlg->SetFileTypeIndex(*typeIndex);
        }
        HRESULT hr = dlg->Show(NULL);
        if (hr == S_OK)
        {
            IShellItem* item = NULL;
            hr = dlg->GetResult(&item);
            if (hr == S_OK)
            {
                LPWSTR name = NULL;
                hr = item->GetDisplayName(SIGDN_FILESYSPATH, &name);
                if (hr == S_OK)
                {
                    *path = name;
                    CoTaskMemFree(name);
                }
                item->Release();
                UINT index = 0;
                dlg->GetFileTypeIndex(&index);
                *typeIndex = index;
            }
        }
        dlg->Release();
        CoUninitialize();
        return (unsigned)hr;
    }

    CoUninitialize();
    String titleCopy;
    titleCopy.append(title, (unsigned)-1);
    String filter;
    if (specs)
    {
        for (int i = 0; i < specCount; ++i)
        {
            filter << specs[i].pszName;
            filter << (wchar_t)0;
            filter << specs[i].pszSpec;
            filter << (wchar_t)0;
        }
    }
    filter << (wchar_t)0;
    filter << (wchar_t)0;
    int err = FileDialogs_saveLegacy(owner, path, filter.m_buffer, typeIndex, titleCopy.m_buffer);
    if (!err)
    {
        const wchar_t* ext;
        if (defaultExt)
        {
            path->removeExtension();
            *path << L'.';
            ext = defaultExt;
            *path << ext;
        }
        else if (specs && (int)*typeIndex >= 1 && (ext = specs[*typeIndex - 1].pszSpec) != NULL)
        {
            path->removeExtension();
            *path << (ext + 1);
        }
    }
    return (unsigned)err;
}
