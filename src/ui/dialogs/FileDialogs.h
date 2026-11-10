#pragma once
#include <windows.h>
#include <shobjidl.h>
#include "SelectFolderFilter.h"

class String;

bool FileDialogs_shellItemApiMissing();
bool FileDialogs_openLegacy(HWND owner, String* path, const wchar_t* filter, const wchar_t* title, bool multiSelect);
int FileDialogs_saveLegacy(HWND owner, String* path, const wchar_t* filter, DWORD* filterIndex, const wchar_t* title);
int CALLBACK FileDialogs_browseCallback(HWND hWnd, UINT msg, LPARAM lParam, LPARAM lpData);
bool FileDialogs_browseFolderLegacy(HWND owner, String* path, const wchar_t* title, int unused, bool newStyle,
                                    FolderValidator validator);
bool FileDialogs_getItemPath(IShellItem* item, String* path);
bool FileDialogs_selectFolder(HWND owner, String* path, const wchar_t* title, int unused, bool newStyle,
                              FolderValidator validator);
bool FileDialogs_open(HWND owner, String* path, const wchar_t* filter, const wchar_t* title, bool multiSelect);
unsigned FileDialogs_save(HWND owner, String* path, const wchar_t* defaultExt, const COMDLG_FILTERSPEC* specs,
                          int specCount, DWORD* typeIndex, const wchar_t* title);

extern HRESULT (WINAPI* g_SHCreateItemFromParsingName)(PCWSTR, IBindCtx*, REFIID, void**);
extern DWORD g_shellItemApiError;
struct BrowseFolderParams
{
    FolderValidator validator;
    const wchar_t*  title;
    const wchar_t*  initialPath;
};
extern BrowseFolderParams g_browseFolderParams;
