#include "menus.h"
#include <cstring>
#include <cwchar>
#include "_imports_p10.h"

HMODULE g_hUxTheme;
GetBufferedPaintBitsFn g_pfnGetBufferedPaintBits;
BeginBufferedPaintFn   g_pfnBeginBufferedPaint;
EndBufferedPaintFn     g_pfnEndBufferedPaint;

void clearMenu(HMENU menu)
{
    if (menu && GetMenuItemCount(menu) > 0)
    {
        do
            DeleteMenu(menu, 0, MF_BYPOSITION);
        while (GetMenuItemCount(menu) > 0);
    }
}

HMENU findScriptsMenu(HMENU menu)
{
    if (!menu)
        return 0;
    MENUITEMINFOW mii;
    wchar_t caption[66];
    memset(&mii, 0, sizeof(mii));
    mii.cbSize = sizeof(MENUITEMINFOW);
    mii.fMask = MIIM_SUBMENU | MIIM_STRING;
    mii.fType = 0;
    mii.dwTypeData = caption;
    mii.cch = 64;
    int count = GetMenuItemCount(menu);
    UINT i = 0;
    if (count <= 0)
        return 0;
    while (true)
    {
        memset(caption, 0, 0x80);
        mii.cch = 64;
        if (GetMenuItemInfoW(menu, i, TRUE, &mii) && mii.hSubMenu && !_wcsicmp(L"Scripts", caption))
            break;
        if ((int)++i >= count)
            return 0;
    }
    return mii.hSubMenu;
}

ULONG_PTR getMenuItemData(HMENU menu, UINT id)
{
    if (!menu)
        return 0;
    MENUITEMINFOW mii;
    memset(&mii, 0, sizeof(mii));
    mii.cbSize = sizeof(MENUITEMINFOW);
    mii.fMask = MIIM_DATA;
    BOOL ok = GetMenuItemInfoW(menu, id, FALSE, &mii);
    ULONG_PTR data = mii.dwItemData;
    if (!ok)
        return 0;
    return data;
}

BOOL deleteMenuItemByData(HMENU menu, ULONG_PTR data)
{
    MENUITEMINFOW mii;
    memset(&mii, 0, sizeof(mii));
    mii.cbSize = sizeof(MENUITEMINFOW);
    mii.fMask = MIIM_DATA;
    mii.dwItemData = 0;
    int result = GetMenuItemCount(menu);
    int count = result;
    UINT i = 0;
    if (result > 0)
    {
        while (true)
        {
            mii.dwItemData = 0;
            result = GetMenuItemInfoW(menu, i, TRUE, &mii);
            if (result == 1)
            {
                result = (int)data;
                if (mii.dwItemData == data)
                    break;
            }
            if ((int)++i >= count)
                return result;
        }
        return DeleteMenu(menu, i, MF_BYPOSITION);
    }
    return result;
}

BOOL setMenuItemTextByData(HMENU menu, ULONG_PTR data, LPWSTR text)
{
    MENUITEMINFOW mii;
    memset(&mii, 0, sizeof(mii));
    mii.cbSize = sizeof(MENUITEMINFOW);
    mii.fMask = MIIM_DATA;
    mii.dwItemData = 0;
    int result = GetMenuItemCount(menu);
    int count = result;
    UINT i = 0;
    if (result > 0)
    {
        while (true)
        {
            mii.dwItemData = 0;
            result = GetMenuItemInfoW(menu, i, TRUE, &mii);
            if (result == 1)
            {
                result = (int)data;
                if (mii.dwItemData == data)
                    break;
            }
            if ((int)++i >= count)
                return result;
        }
        memset(&mii, 0, sizeof(mii));
        mii.dwTypeData = text;
        mii.fMask = MIIM_STRING;
        mii.fType = 0;
        return SetMenuItemInfoW(menu, i, TRUE, &mii);
    }
    return result;
}

BOOL setMenuItemState(HMENU menu, UINT id, int state)
{
    MENUITEMINFOW mii;
    memset(&mii.fMask, 0, sizeof(mii) - sizeof(mii.cbSize));
    mii.cbSize = sizeof(MENUITEMINFOW);
    mii.fMask = MIIM_STATE;
    if (state)
    {
        if (state == 1)
        {
            mii.fState = 0;
            return SetMenuItemInfoW(menu, id, FALSE, &mii);
        }
        if (state == 2)
        {
            mii.fState = MFS_CHECKED;
            return SetMenuItemInfoW(menu, id, FALSE, &mii);
        }
    }
    else
    {
        mii.fState = MFS_GRAYED;
    }
    return SetMenuItemInfoW(menu, id, FALSE, &mii);
}

BOOL checkMenuItemIfEnabled(HMENU menu, UINT id, bool checked)
{
    MENUITEMINFOW mii;
    memset(&mii.fMask, 0, sizeof(mii) - sizeof(mii.cbSize));
    mii.cbSize = sizeof(MENUITEMINFOW);
    mii.fMask = MIIM_STATE;
    BOOL result = GetMenuItemInfoW(menu, id, FALSE, &mii);
    if ((mii.fState & MFS_GRAYED) == 0)
    {
        UINT state = 0;
        if (checked)
            state = MFS_CHECKED;
        mii.fState = state;
        return SetMenuItemInfoW(menu, id, FALSE, &mii);
    }
    return result;
}

HRESULT create32BitHBITMAP(HDC hdc, const SIZE* size, HBITMAP* bitmap)
{
    *bitmap = 0;
    BITMAPINFO bmi;
    memset(&bmi, 0, sizeof(bmi));
    bmi.bmiHeader.biWidth = size->cx;
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biHeight = size->cy;
    HDC dc = hdc ? hdc : GetDC(0);
    if (dc)
    {
        *bitmap = CreateDIBSection(dc, &bmi, DIB_RGB_COLORS, 0, 0, 0);
        if (hdc != dc)
            ReleaseDC(0, dc);
    }
    HRESULT result = 0;
    if (!*bitmap)
        result = E_OUTOFMEMORY;
    return result;
}

bool hasAlpha(const DWORD* argb, const SIZE* size, int cxRow)
{
    int cx = size->cx;
    int cy = size->cy;
    int delta = cxRow - size->cx;
    if (!cy)
        return false;
    while (true)
    {
        int x = cx;
        if (x)
        {
            while (true)
            {
                DWORD pixel = *argb++;
                if ((pixel & 0xFF000000) != 0)
                    return true;
                if (!--x)
                    break;
            }
        }
        argb += delta;
        if (!--cy)
            return false;
    }
}

HRESULT convertToPARGB32(HDC hdc, DWORD* argb, HBITMAP mask, const SIZE* size, int cxRow)
{
    BITMAPINFO bmi;
    memset(&bmi, 0, sizeof(bmi));
    bmi.bmiHeader.biWidth = size->cx;
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biHeight = size->cy;
    HANDLE heap = GetProcessHeap();
    DWORD* bits = (DWORD*)HeapAlloc(heap, 0, 4 * bmi.bmiHeader.biWidth * bmi.bmiHeader.biHeight);
    if (!bits)
        return E_OUTOFMEMORY;
    HRESULT result = E_UNEXPECTED;
    if (GetDIBits(hdc, mask, 0, bmi.bmiHeader.biHeight, bits, &bmi, DIB_RGB_COLORS) == bmi.bmiHeader.biHeight)
    {
        int width = bmi.bmiHeader.biWidth;
        int height = bmi.bmiHeader.biHeight;
        const DWORD* maskPixel = bits;
        if (height)
        {
            int delta = cxRow - bmi.bmiHeader.biWidth;
            do
            {
                if (width)
                {
                    do
                    {
                        if (*maskPixel++)
                            *argb = 0;
                        else
                            *argb |= 0xFF000000;
                        ++argb;
                        --width;
                    } while (width);
                    width = bmi.bmiHeader.biWidth;
                }
                argb += delta;
                --height;
            } while (height);
        }
        result = S_OK;
    }
    HeapFree(heap, 0, bits);
    return result;
}

HRESULT convertBufferToPARGB32(HPAINTBUFFER paintBuffer, HDC hdc, HICON icon, const SIZE* size)
{
    RGBQUAD* quads = 0;
    int cxRow = 0;
    HRESULT result = g_pfnGetBufferedPaintBits(paintBuffer, &quads, &cxRow);
    HRESULT hr = result;
    if (result >= 0)
    {
        DWORD* argb = (DWORD*)quads;
        if (!hasAlpha(argb, size, cxRow))
        {
            ICONINFO info;
            memset(&info, 0, sizeof(info));
            if (GetIconInfo(icon, &info))
            {
                if (info.hbmMask)
                    hr = convertToPARGB32(hdc, argb, info.hbmMask, size, cxRow);
                DeleteObject(info.hbmColor);
                DeleteObject(info.hbmMask);
            }
        }
        return hr;
    }
    return result;
}

HBITMAP iconToBitmapPARGB32(HICON icon)
{
    if (!icon)
        return 0;
    EndBufferedPaintFn endBufferedPaint;
    HMODULE module;
    if (g_hUxTheme || (module = LoadLibraryW(L"UXTHEME.DLL"), (g_hUxTheme = module) == 0))
    {
        endBufferedPaint = g_pfnEndBufferedPaint;
    }
    else
    {
        g_pfnGetBufferedPaintBits = (GetBufferedPaintBitsFn)GetProcAddress(module, "GetBufferedPaintBits");
        g_pfnBeginBufferedPaint = (BeginBufferedPaintFn)GetProcAddress(g_hUxTheme, "BeginBufferedPaint");
        endBufferedPaint = (EndBufferedPaintFn)GetProcAddress(g_hUxTheme, "EndBufferedPaint");
        g_pfnEndBufferedPaint = endBufferedPaint;
    }
    if (!g_pfnGetBufferedPaintBits || !endBufferedPaint)
        return 0;
    SIZE size;
    size.cx = GetSystemMetrics(SM_CXSMICON);
    size.cy = GetSystemMetrics(SM_CYSMICON);
    RECT rc;
    SetRect(&rc, 0, 0, size.cx, size.cy);
    HBITMAP bitmap = 0;
    HDC hdc = CreateCompatibleDC(0);
    if (hdc)
    {
        HBITMAP created = 0;
        HRESULT hr = create32BitHBITMAP(hdc, &size, &created);
        bitmap = created;
        if (hr >= 0)
        {
            HGDIOBJ old = SelectObject(hdc, created);
            if (old)
            {
                BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
                BP_PAINTPARAMS params;
                params.cbSize = sizeof(BP_PAINTPARAMS);
                params.dwFlags = BPPF_ERASE;
                params.prcExclude = 0;
                params.pBlendFunction = &blend;
                HDC hdcBuffer = 0;
                HPAINTBUFFER paintBuffer = g_pfnBeginBufferedPaint(hdc, &rc, BPBF_DIB, &params, &hdcBuffer);
                if (paintBuffer)
                {
                    if (DrawIconEx(hdcBuffer, 0, 0, icon, size.cx, size.cy, 0, 0, DI_NORMAL))
                        convertBufferToPARGB32(paintBuffer, hdc, icon, &size);
                    g_pfnEndBufferedPaint(paintBuffer, TRUE);
                }
                SelectObject(hdc, old);
            }
        }
        DeleteDC(hdc);
    }
    return bitmap;
}
