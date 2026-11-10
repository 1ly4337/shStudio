#pragma once
#include <windows.h>
#include <commctrl.h>

DWORD   getBitmapInfo(HGDIOBJ bitmap, BITMAPINFO* info);
BOOL    drawBitmapAligned(HDC hdc, const int* rect, HGDIOBJ bitmap, char flags);
BOOL    drawBitmapTransparent(HDC hdc, const POINT* pos, HGDIOBJ bitmap);
HBITMAP createSolidBitmap(int cx, int cy, COLORREF color);
HBITMAP captureClientArea(HWND hwnd);
HBITMAP cropBitmap(HGDIOBJ bitmap, int x, int y, int cx, int cy);
HBITMAP copyBitmap(const HGDIOBJ* bitmap);
HBITMAP halveBitmap(HGDIOBJ bitmap);
void*   getBitmapBits(HBITMAP bitmap);
RGBQUAD sampleBitmap(const BYTE* pixels, float x, const int* size, float y);
HBITMAP resizeBitmap(HBITMAP bitmap, int cx, int cy);
HBITMAP stripToRow(HGDIOBJ bitmap, const int* cellSize);
SIZE    getBitmapSize(HGDIOBJ bitmap);
bool    isBlankIcon(HGDIOBJ bitmap, int cx, int cy);
HBITMAP imageListBitmap(HIMAGELIST list, int index);
HBITMAP createGradientBitmap(int height, COLORREF top, COLORREF bottom);
HBITMAP blendDiagonal(HBITMAP first, HBITMAP second, int mode);
HBITMAP blendLinear(HBITMAP first, HBITMAP second, char vertical);
HBITMAP blendCorners(const HBITMAP* bitmaps, long corners);
