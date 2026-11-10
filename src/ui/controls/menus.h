#pragma once
#include <windows.h>
#include <uxtheme.h>

void clearMenu(HMENU menu);
HMENU findScriptsMenu(HMENU menu);
ULONG_PTR getMenuItemData(HMENU menu, UINT id);
BOOL deleteMenuItemByData(HMENU menu, ULONG_PTR data);
BOOL setMenuItemTextByData(HMENU menu, ULONG_PTR data, LPWSTR text);
BOOL setMenuItemState(HMENU menu, UINT id, int state);
BOOL checkMenuItemIfEnabled(HMENU menu, UINT id, bool checked);

HRESULT create32BitHBITMAP(HDC hdc, const SIZE* size, HBITMAP* bitmap);
bool hasAlpha(const DWORD* argb, const SIZE* size, int cxRow);
HRESULT convertToPARGB32(HDC hdc, DWORD* argb, HBITMAP mask, const SIZE* size, int cxRow);
HRESULT convertBufferToPARGB32(HPAINTBUFFER paintBuffer, HDC hdc, HICON icon, const SIZE* size);
HBITMAP iconToBitmapPARGB32(HICON icon);

extern HMODULE g_hUxTheme;
typedef HRESULT (WINAPI* GetBufferedPaintBitsFn)(HPAINTBUFFER, RGBQUAD**, int*);
typedef HPAINTBUFFER (WINAPI* BeginBufferedPaintFn)(HDC, const RECT*, BP_BUFFERFORMAT, BP_PAINTPARAMS*, HDC*);
typedef HRESULT (WINAPI* EndBufferedPaintFn)(HPAINTBUFFER, BOOL);
extern GetBufferedPaintBitsFn g_pfnGetBufferedPaintBits;
extern BeginBufferedPaintFn   g_pfnBeginBufferedPaint;
extern EndBufferedPaintFn     g_pfnEndBufferedPaint;
