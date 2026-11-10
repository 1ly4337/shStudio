#include "gdiutil.h"
#include "DIBPixels.h"
#include <cstring>
#include <new>

#pragma comment(lib, "msimg32.lib")
#pragma comment(lib, "comctl32.lib")

DWORD getBitmapInfo(HGDIOBJ bitmap, BITMAPINFO* info)
{
    memset(info, 0, 40);
    if (!bitmap)
        return 13;
    BITMAP bm;
    if (!GetObjectW(bitmap, sizeof(BITMAP), &bm))
        return GetLastError();
    if (!bm.bmHeight)
        return 13;
    int bits = bm.bmPlanes * bm.bmBitsPixel;
    int depth;
    if (bits == 1)
        depth = 1;
    else if (bits > 4)
    {
        if (bits > 8)
        {
            if (bits > 16)
                depth = 8 * (bits > 24) + 24;
            else
                depth = 16;
        }
        else
            depth = 8;
    }
    else
        depth = 4;
    BITMAPINFOHEADER& header = info->bmiHeader;
    header.biWidth = bm.bmWidth;
    header.biPlanes = bm.bmPlanes;
    header.biSize = 40;
    header.biHeight = bm.bmHeight;
    header.biBitCount = bm.bmBitsPixel;
    header.biCompression = BI_RGB;
    header.biSizeImage = bm.bmHeight * ((int)((depth * bm.bmWidth + 31) & 0xFFFFFFE0) / 8);
    header.biXPelsPerMeter = 96;
    header.biYPelsPerMeter = 96;
    if (depth >= 24)
    {
        header.biClrUsed = 0;
        header.biClrImportant = 0;
    }
    else
    {
        header.biClrImportant = 0;
        header.biClrUsed = 1 << depth;
    }
    return 0;
}

BOOL drawBitmapAligned(HDC hdc, const int* rect, HGDIOBJ bitmap, char flags)
{
    HDC memDC = CreateCompatibleDC(hdc);
    if (!memDC)
        return FALSE;
    HGDIOBJ old = SelectObject(memDC, bitmap);
    BITMAP bm;
    GetObjectW(bitmap, sizeof(BITMAP), &bm);
    int x;
    if (flags & 8)
        x = rect[2] - bm.bmWidth;
    else if (flags & 2)
        x = rect[0];
    else
        x = (rect[2] - bm.bmWidth) / 2;
    int y;
    if (flags & 4)
        y = rect[3] - bm.bmHeight;
    else if (flags & 1)
        y = rect[1];
    else
        y = (rect[3] - bm.bmHeight) / 2;
    if (x < 0 || y < 0)
        StretchBlt(hdc, 0, 0, rect[2], rect[3], memDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
    else
        BitBlt(hdc, x, y, rect[2], rect[3], memDC, 0, 0, SRCCOPY);
    SelectObject(memDC, old);
    return DeleteDC(memDC);
}

BOOL drawBitmapTransparent(HDC hdc, const POINT* pos, HGDIOBJ bitmap)
{
    HDC srcDC = CreateCompatibleDC(hdc);
    if (!srcDC)
        return FALSE;
    HGDIOBJ oldSrc = SelectObject(srcDC, bitmap);
    COLORREF key = GetPixel(srcDC, 0, 0);
    BITMAP bm;
    GetObjectW(bitmap, sizeof(BITMAP), &bm);
    POINT size = { bm.bmWidth, bm.bmHeight };
    DPtoLP(srcDC, &size, 1);
    HDC saveDC = CreateCompatibleDC(hdc);
    HDC invMaskDC = CreateCompatibleDC(hdc);
    HDC maskDC = CreateCompatibleDC(hdc);
    HDC workDC = CreateCompatibleDC(hdc);
    HGDIOBJ saveBitmap = CreateCompatibleBitmap(hdc, size.x, size.y);
    HBITMAP invMaskBitmap = CreateBitmap(size.x, size.y, 1, 1, 0);
    HBITMAP maskBitmap = CreateBitmap(size.x, size.y, 1, 1, 0);
    HBITMAP workBitmap = CreateCompatibleBitmap(hdc, size.x, size.y);
    saveBitmap = SelectObject(saveDC, saveBitmap);
    HGDIOBJ oldInvMask = SelectObject(invMaskDC, invMaskBitmap);
    HGDIOBJ oldMask = SelectObject(maskDC, maskBitmap);
    HGDIOBJ oldWork = SelectObject(workDC, workBitmap);
    SetMapMode(srcDC, GetMapMode(hdc));
    BitBlt(saveDC, 0, 0, size.x, size.y, srcDC, 0, 0, SRCCOPY);
    COLORREF oldBk = SetBkColor(srcDC, key);
    BitBlt(maskDC, 0, 0, size.x, size.y, srcDC, 0, 0, SRCCOPY);
    SetBkColor(srcDC, oldBk);
    BitBlt(invMaskDC, 0, 0, size.x, size.y, maskDC, 0, 0, NOTSRCCOPY);
    BitBlt(workDC, 0, 0, size.x, size.y, hdc, pos->x, pos->y, SRCCOPY);
    BitBlt(workDC, 0, 0, size.x, size.y, maskDC, 0, 0, SRCAND);
    BitBlt(srcDC, 0, 0, size.x, size.y, invMaskDC, 0, 0, SRCAND);
    BitBlt(workDC, 0, 0, size.x, size.y, srcDC, 0, 0, SRCPAINT);
    BitBlt(hdc, pos->x, pos->y, size.x, size.y, workDC, 0, 0, SRCCOPY);
    BitBlt(srcDC, 0, 0, size.x, size.y, saveDC, 0, 0, SRCCOPY);
    DeleteObject(SelectObject(saveDC, saveBitmap));
    DeleteObject(SelectObject(workDC, oldWork));
    DeleteObject(SelectObject(maskDC, oldMask));
    DeleteObject(SelectObject(invMaskDC, oldInvMask));
    DeleteDC(saveDC);
    DeleteDC(workDC);
    DeleteDC(maskDC);
    DeleteDC(invMaskDC);
    SelectObject(srcDC, oldSrc);
    return DeleteDC(srcDC);
}

HBITMAP createSolidBitmap(int cx, int cy, COLORREF color)
{
    HDC display = CreateDCW(L"DISPLAY", 0, 0, 0);
    HDC memDC = CreateCompatibleDC(display);
    HBITMAP bitmap = CreateCompatibleBitmap(display, cx, cy);
    HGDIOBJ old = SelectObject(memDC, bitmap);
    HBRUSH brush = CreateSolidBrush(color);
    RECT rc;
    rc.right = cx;
    rc.left = 0;
    rc.top = 0;
    rc.bottom = cy;
    UnrealizeObject(brush);
    FillRect(memDC, &rc, brush);
    DeleteObject(brush);
    SelectObject(memDC, old);
    DeleteDC(memDC);
    DeleteDC(display);
    return bitmap;
}

HBITMAP captureClientArea(HWND hwnd)
{
    RECT rc;
    GetClientRect(hwnd, &rc);
    int cx = rc.right;
    int cy = rc.bottom;
    HDC windowDC = GetDC(hwnd);
    if (windowDC)
    {
        HDC display = CreateDCW(L"DISPLAY", 0, 0, 0);
        HDC memDC = CreateCompatibleDC(display);
        HBITMAP bitmap = CreateCompatibleBitmap(display, cx, cy);
        HGDIOBJ old = SelectObject(memDC, bitmap);
        bool copied = BitBlt(memDC, 0, 0, cx, cy, windowDC, 0, 0, SRCCOPY) != 0;
        SelectObject(memDC, old);
        DeleteDC(memDC);
        DeleteDC(windowDC);
        DeleteDC(display);
        if (copied)
            return bitmap;
        DeleteObject(bitmap);
    }
    return 0;
}

HBITMAP cropBitmap(HGDIOBJ bitmap, int x, int y, int cx, int cy)
{
    if (!bitmap)
        return 0;
    BITMAP bm;
    memset(&bm, 0, sizeof(bm));
    if (!GetObjectW(bitmap, sizeof(BITMAP), &bm))
        return 0;
    if (x < 0 || cx < 0 || x + cx > bm.bmWidth || y < 0 || cy < 0 || y + cy > bm.bmHeight)
        return 0;
    HDC display = CreateDCW(L"DISPLAY", 0, 0, 0);
    HDC srcDC = CreateCompatibleDC(display);
    HGDIOBJ oldSrc = SelectObject(srcDC, bitmap);
    HDC dstDC = CreateCompatibleDC(display);
    HBITMAP result = CreateCompatibleBitmap(display, cx, cy);
    HGDIOBJ oldDst = SelectObject(dstDC, result);
    bool copied = BitBlt(dstDC, 0, 0, cx, cy, srcDC, x, y, SRCCOPY) != 0;
    SelectObject(dstDC, oldDst);
    DeleteDC(dstDC);
    SelectObject(srcDC, oldSrc);
    DeleteDC(srcDC);
    DeleteDC(display);
    if (copied)
        return result;
    DeleteObject(result);
    return 0;
}

HBITMAP copyBitmap(const HGDIOBJ* bitmap)
{
    if (!*bitmap)
        return 0;
    BITMAP bm;
    memset(&bm, 0, sizeof(bm));
    if (!GetObjectW(*bitmap, sizeof(BITMAP), &bm))
        return 0;
    int cy = bm.bmHeight;
    int cx = bm.bmWidth;
    HDC display = CreateDCW(L"DISPLAY", 0, 0, 0);
    HDC srcDC = CreateCompatibleDC(display);
    HGDIOBJ oldSrc = SelectObject(srcDC, *bitmap);
    HDC dstDC = CreateCompatibleDC(display);
    HBITMAP result = CreateCompatibleBitmap(display, cx, cy);
    HGDIOBJ oldDst = SelectObject(dstDC, result);
    bool copied = BitBlt(dstDC, 0, 0, cx, cy, srcDC, 0, 0, SRCCOPY) != 0;
    SelectObject(dstDC, oldDst);
    DeleteDC(dstDC);
    SelectObject(srcDC, oldSrc);
    DeleteDC(srcDC);
    DeleteDC(display);
    if (copied)
        return result;
    DeleteObject(result);
    return 0;
}

HBITMAP halveBitmap(HGDIOBJ bitmap)
{
    BITMAP bm;
    memset(&bm, 0, sizeof(bm));
    if (!GetObjectW(bitmap, sizeof(BITMAP), &bm))
        return 0;
    int cx = bm.bmWidth / 2;
    int cy = bm.bmHeight / 2;
    DIBPixels src;
    DIBPixels dst;
    if (!src.load((HBITMAP)bitmap) || !dst.create(0, cx, cy))
        return 0;
    for (int y = 0; y < cy; ++y)
    {
        for (int x = 0; x < cx; ++x)
        {
            int sum2 = 0;
            int sum1 = 0;
            int sum0 = 0;
            for (int i = 0; i < 2; ++i)
            {
                for (int j = 0; j < 2; ++j)
                {
                    const BYTE* p = src.pixel(2 * x + i, 2 * y + j);
                    sum2 += p[2];
                    sum1 += p[1];
                    sum0 += p[0];
                }
            }
            BYTE* q = dst.pixel(x, y);
            q[2] = (BYTE)(sum2 / 4);
            q[1] = (BYTE)(sum1 / 4);
            q[0] = (BYTE)(sum0 / 4);
        }
    }
    HDC screen = GetDC(0);
    HBITMAP result = CreateCompatibleBitmap(screen, cx, cy);
    BITMAPINFO bmi;
    if (result && !getBitmapInfo(result, &bmi) && !SetDIBits(screen, result, 0, cy, dst.m_bits, &bmi, DIB_RGB_COLORS))
    {
        DeleteObject(result);
        result = 0;
    }
    ReleaseDC(0, screen);
    return result;
}

void* getBitmapBits(HBITMAP bitmap)
{
    BITMAPINFO bmi;
    if (getBitmapInfo(bitmap, &bmi))
        return 0;
    HDC screen = GetDC(0);
    if (!screen)
        return 0;
    void* bits = ::operator new[](bmi.bmiHeader.biSizeImage);
    if (!GetDIBits(screen, bitmap, 0, bmi.bmiHeader.biHeight, bits, &bmi, DIB_RGB_COLORS))
    {
        ::operator delete[](bits);
        bits = 0;
    }
    ReleaseDC(0, screen);
    return bits;
}

RGBQUAD sampleBitmap(const BYTE* pixels, float x, const int* size, float y)
{
    RGBQUAD out;
    int ix = (int)x;
    int iy = (int)y;
    if (x < 0.0f || y < 0.0f)
    {
        if (iy < 0)
            iy = 0;
        int row = size[0] * iy;
        if (ix < 0)
            ix = 0;
        *(DWORD*)&out = ((const DWORD*)pixels)[ix + row];
        return out;
    }
    float fx = x - (float)ix;
    int row = iy * size[0];
    float fy = y - (float)iy;
    int i00 = row + ix;
    int x1 = size[0] - 1;
    if (ix + 1 < x1)
        x1 = ix + 1;
    int i10 = row + x1;
    float wx = (float)(1.0 - fx);
    int y1 = size[1] - 1;
    if (iy + 1 < y1)
        y1 = iy + 1;
    int i01 = ix + size[0] * y1;
    float wy = (float)(1.0 - fy);
    const BYTE* p00 = pixels + 4 * i00;
    const BYTE* p10 = pixels + 4 * i10;
    const BYTE* p01 = pixels + 4 * i01;
    for (int c = 2; c >= 0; c -= 2)
    {
        float v00 = (float)p00[c];
        ((BYTE*)&out)[c] = (BYTE)(int)(((((float)p10[c] * fx) + (v00 * wx)) + (v00 * wy) + ((float)p01[c] * fy)) * 0.5f);
    }
    out.rgbReserved = 0xFF;
    {
        float v00 = (float)p00[1];
        out.rgbGreen = (BYTE)(int)(((((float)p10[1] * fx) + (v00 * wx)) + (v00 * wy) + ((float)p01[1] * fy)) * 0.5f);
    }
    return out;
}

HBITMAP resizeBitmap(HBITMAP bitmap, int cx, int cy)
{
    if (!bitmap || !cx || !cy)
        return 0;
    int count = cx * cy;
    BYTE* src = (BYTE*)getBitmapBits(bitmap);
    DWORD* dst = new DWORD[count]();
    if (!src)
    {
        if (dst)
            delete[] dst;
        return 0;
    }
    if (!dst)
    {
        ::operator delete[](src);
        return 0;
    }
    SIZE srcSize = getBitmapSize(bitmap);
    int width = srcSize.cx;
    int height = srcSize.cy;
    int dims[2] = { width, height };
    if (width >= cx || height >= cy)
    {
        float stepX = (float)width / (float)cx;
        float stepY = (float)height / (float)cy;
        int hiX = -1;
        float accX = stepX * -0.5f;
        BYTE* column = (BYTE*)dst + 1;
        for (int x = 0; x < cx; ++x, column += 4)
        {
            accX = accX + stepX;
            int loX = hiX + 1;
            int hiY = -1;
            float accY = stepY * -0.5f;
            hiX = (int)accX + 1;
            if (width - 1 < hiX)
                hiX = width - 1;
            if (loX > hiX)
                continue;
            BYTE* out = column;
            for (int y = 0; y < cy; ++y, out += 4 * cx)
            {
                accY = accY + stepY;
                int loY = hiY + 1;
                hiY = (int)accY + 1;
                if (height - 1 < hiY)
                    hiY = height - 1;
                if (loY > hiY)
                    continue;
                const BYTE* first = src + 4 * loX + 1 + 4 * loY * width;
                int rows = hiY - loY + 1;
                int sum2 = 0;
                int sum1 = 0;
                int sum0 = 0;
                int n = 0;
                for (int c = hiX - loX + 1; c > 0; --c, first += 4)
                {
                    n += rows;
                    const BYTE* p = first;
                    for (int r = rows; r > 0; --r, p += 4 * width)
                    {
                        sum2 += p[1];
                        sum1 += p[0];
                        sum0 += p[-1];
                    }
                }
                out[1] = (BYTE)(sum2 / n);
                out[0] = (BYTE)(sum1 / n);
                out[-1] = (BYTE)(sum0 / n);
            }
        }
    }
    else
    {
        float stepX = (float)width / (float)cx;
        float stepY = (float)height / (float)cy;
        float fx = 0.0f;
        for (int x = 0; x < cx; ++x)
        {
            float fy = 0.0f;
            DWORD* out = dst + x;
            for (int y = 0; y < cy; ++y)
            {
                RGBQUAD q = sampleBitmap(src, fx, dims, fy);
                fy = fy + stepY;
                *out = *(DWORD*)&q;
                out += cx;
            }
            fx = fx + stepX;
        }
    }
    HBITMAP result = 0;
    HDC screen = GetDC(0);
    if (screen)
    {
        result = CreateCompatibleBitmap(screen, cx, cy);
        BITMAPINFO bmi;
        if (result && (getBitmapInfo(result, &bmi) || !SetDIBits(screen, result, 0, cy, dst, &bmi, DIB_RGB_COLORS)))
        {
            DeleteObject(result);
            result = 0;
        }
        ReleaseDC(0, screen);
    }
    ::operator delete[](src);
    delete[] dst;
    return result;
}

HBITMAP stripToRow(HGDIOBJ bitmap, const int* cellSize)
{
    HGDIOBJ source = bitmap;
    if (!bitmap)
        return 0;
    BITMAP bm;
    memset(&bm, 0, sizeof(bm));
    if (!GetObjectW(bitmap, sizeof(BITMAP), &bm))
        return 0;
    int height = bm.bmHeight;
    int width = bm.bmWidth;
    if (height == cellSize[1])
        return copyBitmap(&source);
    HDC display = CreateDCW(L"DISPLAY", 0, 0, 0);
    HDC srcDC = CreateCompatibleDC(display);
    HGDIOBJ oldSrc = SelectObject(srcDC, bitmap);
    int cellHeight = cellSize[1];
    int rowWidth = MulDiv(cellSize[0], width * height, cellHeight * cellSize[0]);
    HDC dstDC = CreateCompatibleDC(display);
    HBITMAP result = CreateCompatibleBitmap(display, rowWidth, cellHeight);
    int y = 0;
    int x = 0;
    HGDIOBJ oldDst = SelectObject(dstDC, result);
    bool failed = false;
    if (height > 0)
    {
        for (;;)
        {
            if (!BitBlt(dstDC, x, 0, width, cellSize[1], srcDC, 0, y, SRCCOPY))
            {
                failed = true;
                break;
            }
            y += cellSize[1];
            x += width;
            if (y >= height)
                break;
        }
    }
    SelectObject(dstDC, oldDst);
    DeleteDC(dstDC);
    SelectObject(srcDC, oldSrc);
    DeleteDC(srcDC);
    DeleteDC(display);
    if (failed)
    {
        DeleteObject(result);
        return 0;
    }
    return result;
}

SIZE getBitmapSize(HGDIOBJ bitmap)
{
    SIZE size = { 0, 0 };
    if (bitmap)
    {
        BITMAP bm;
        memset(&bm, 0, sizeof(bm));
        GetObjectW(bitmap, sizeof(BITMAP), &bm);
        size.cx = bm.bmWidth;
        size.cy = bm.bmHeight;
    }
    return size;
}

static inline bool isBright(COLORREF c)
{
    return GetRValue(c) > 0x60 || GetGValue(c) > 0x60 || GetBValue(c) > 0x60;
}

bool isBlankIcon(HGDIOBJ bitmap, int cx, int cy)
{
    if (!bitmap)
        return false;
    HDC display = CreateDCW(L"DISPLAY", 0, 0, 0);
    HDC memDC = CreateCompatibleDC(display);
    HGDIOBJ old = SelectObject(memDC, bitmap);
    bool bright = false;
    for (int i = 3; i < cx - 3; ++i)
    {
        if (isBright(GetPixel(memDC, i, i)))
        {
            bright = true;
            break;
        }
    }
    if (!bright)
    {
        for (int i = 3; i < cx - 3; ++i)
        {
            if (isBright(GetPixel(memDC, i, cx - i - 1)))
            {
                bright = true;
                break;
            }
        }
    }
    int red = 0;
    int green = 0;
    int blue = 0;
    if (!bright)
    {
        int sumGreen = 0;
        int n = 0;
        int midY = cy / 2;
        for (int x = cx / 2 - 4; x <= cx / 2 + 4 && !bright; ++x)
        {
            for (int y = midY - 4; y <= midY + 4; ++y)
            {
                COLORREF c = GetPixel(memDC, x, y);
                if (isBright(c))
                {
                    bright = true;
                    break;
                }
                ++n;
                red += GetRValue(c);
                sumGreen += GetGValue(c);
                blue += GetBValue(c);
            }
        }
        if (n)
        {
            red /= n;
            green = sumGreen / n;
            blue /= n;
        }
        else
            green = sumGreen;
    }
    SelectObject(memDC, old);
    DeleteDC(memDC);
    DeleteDC(display);
    return !bright && red < 64 && green < 64 && blue < 64;
}

HBITMAP imageListBitmap(HIMAGELIST list, int index)
{
    int count = ImageList_GetImageCount(list);
    int cx;
    int cy;
    if (index < 0 || index >= count || !ImageList_GetIconSize(list, &cx, &cy))
        return 0;
    HBITMAP bitmap = createSolidBitmap(cx, cy, 0xFFFFFF);
    HDC display = CreateDCW(L"DISPLAY", 0, 0, 0);
    HDC memDC = CreateCompatibleDC(display);
    HGDIOBJ old = SelectObject(memDC, bitmap);
    ImageList_DrawEx(list, index, memDC, 0, 0, 0, 0, CLR_NONE, CLR_NONE, ILD_NORMAL);
    SelectObject(memDC, old);
    DeleteDC(memDC);
    DeleteDC(display);
    return bitmap;
}

HBITMAP createGradientBitmap(int height, COLORREF top, COLORREF bottom)
{
    HDC screen = GetDC(0);
    HDC memDC = CreateCompatibleDC(screen);
    HBITMAP bitmap = CreateCompatibleBitmap(screen, 8, height);
    HGDIOBJ old = SelectObject(memDC, bitmap);
    int topGreen = GetGValue(top);
    int topRed = GetRValue(top);
    int deltaRed = GetRValue(top) - GetRValue(bottom);
    int topBlue = GetBValue(top);
    int deltaBlue = GetBValue(top) - GetBValue(bottom);
    int deltaGreen = topGreen - GetGValue(bottom);
    for (int y = 0; y < height; ++y)
    {
        BYTE blue = (BYTE)(topBlue - MulDiv(deltaBlue, y, height));
        BYTE green = (BYTE)(topGreen - MulDiv(deltaGreen, y, height));
        BYTE red = (BYTE)(topRed - MulDiv(deltaRed, y, height));
        HPEN pen = CreatePen(PS_SOLID, 1, RGB(red, green, blue));
        SelectObject(memDC, pen);
        MoveToEx(memDC, 0, y, 0);
        LineTo(memDC, 8, y);
        DeleteObject(pen);
    }
    SelectObject(memDC, old);
    DeleteDC(memDC);
    ReleaseDC(0, screen);
    return bitmap;
}

static bool prepareBlend(HBITMAP& first, HBITMAP& second, int& size, int& cy, HBITMAP& temp)
{
    BITMAP bm;
    memset(&bm, 0, sizeof(bm));
    if (!GetObjectW(first, sizeof(BITMAP), &bm))
        return false;
    size = bm.bmWidth;
    cy = bm.bmHeight;
    if (bm.bmWidth != bm.bmHeight || !GetObjectW(second, sizeof(BITMAP), &bm) || bm.bmWidth != bm.bmHeight)
        return false;
    temp = 0;
    if (size != bm.bmWidth)
    {
        if (size < bm.bmWidth)
        {
            temp = resizeBitmap(second, size, cy);
            if (!temp)
                return false;
            second = temp;
        }
        else
        {
            cy = bm.bmHeight;
            size = bm.bmWidth;
            temp = resizeBitmap(first, bm.bmWidth, bm.bmHeight);
            if (!temp)
                return false;
            first = temp;
        }
    }
    return true;
}

static HBITMAP finishBlend(DIBPixels& dst, int cx, int cy, HBITMAP temp)
{
    HDC screen = GetDC(0);
    HBITMAP result = CreateCompatibleBitmap(screen, cx, cy);
    BITMAPINFO bmi;
    if (result && !getBitmapInfo(result, &bmi) && !SetDIBits(screen, result, 0, cy, dst.m_bits, &bmi, DIB_RGB_COLORS))
    {
        DeleteObject(result);
        result = 0;
    }
    ReleaseDC(0, screen);
    if (temp)
        DeleteObject(temp);
    return result;
}

HBITMAP blendDiagonal(HBITMAP first, HBITMAP second, int mode)
{
    int size;
    int cy;
    HBITMAP temp;
    if (!prepareBlend(first, second, size, cy, temp))
        return 0;
    DIBPixels a;
    DIBPixels b;
    DIBPixels dst;
    if (!a.load(first) || !b.load(second) || !dst.create(0, size, cy))
        return 0;
    float t;
    memcpy(&t, &temp, sizeof(t));
    for (int y = 0; y < cy; ++y)
    {
        float fy = (float)y / (float)cy;
        for (int x = 0; x < size; ++x)
        {
            const BYTE* pa = a.pixel(x, y);
            const BYTE* pb = b.pixel(x, y);
            BYTE* out = dst.pixel(x, y);
            float fx = (float)x / (float)size;
            if (mode == 3)
                t = (float)(1.0 - fx) + fy;
            else if (mode == 4)
                t = (float)(2.0 - fx) - fy;
            if (t >= 0.0f)
            {
                if (t > 1.0f)
                    t = 1.0f;
            }
            else
                t = 0.0f;
            out[2] = (BYTE)(int)(((float)pb[2] * (float)(1.0 - t)) + ((float)pa[2] * t));
            out[1] = (BYTE)(int)(((float)pb[1] * (float)(1.0 - t)) + ((float)pa[1] * t));
            out[0] = (BYTE)(int)(((float)pb[0] * (float)(1.0 - t)) + ((float)pa[0] * t));
        }
    }
    return finishBlend(dst, size, cy, temp);
}

HBITMAP blendLinear(HBITMAP first, HBITMAP second, char vertical)
{
    int size;
    int cy;
    HBITMAP temp;
    if (!prepareBlend(first, second, size, cy, temp))
        return 0;
    DIBPixels a;
    DIBPixels b;
    DIBPixels dst;
    if (!a.load(first) || !b.load(second) || !dst.create(0, size, cy))
        return 0;
    for (int y = 0; y < cy; ++y)
    {
        for (int x = 0; x < size; ++x)
        {
            const BYTE* pa = a.pixel(x, y);
            const BYTE* pb = b.pixel(x, y);
            BYTE* out = dst.pixel(x, y);
            float t;
            if (vertical)
                t = (float)(1.0 - (float)y / (float)cy);
            else
                t = (float)x / (float)size;
            out[2] = (BYTE)(int)(((float)pa[2] * (float)(1.0 - t)) + ((float)pb[2] * t));
            out[1] = (BYTE)(int)(((float)pa[1] * (float)(1.0 - t)) + ((float)pb[1] * t));
            out[0] = (BYTE)(int)(((float)pa[0] * (float)(1.0 - t)) + ((float)pb[0] * t));
        }
    }
    return finishBlend(dst, size, cy, temp);
}

HBITMAP blendCorners(const HBITMAP* bitmaps, long corners)
{
    if (!bitmaps)
        return 0;
    int b = (corners >> 8) & 0xF;
    int a = (corners >> 12) & 0xF;
    int c = (corners >> 4) & 0xF;
    int d = corners & 0xF;
    HBITMAP temp;
    HBITMAP other;
    char vertical;
    if (a == b)
    {
        if (c == d)
            return blendLinear(bitmaps[a], bitmaps[d], 0);
        if (c == b)
            return blendDiagonal(bitmaps[a], bitmaps[d], 4);
        if (d == a)
            return blendDiagonal(bitmaps[a], bitmaps[c], 3);
        temp = blendLinear(bitmaps[d], bitmaps[c], 1);
        other = bitmaps[b];
        vertical = 0;
    }
    else if (a == d)
    {
        if (b == c)
            return blendLinear(bitmaps[a], bitmaps[b], 1);
        temp = blendLinear(bitmaps[b], bitmaps[c], 0);
        other = bitmaps[a];
        vertical = 1;
    }
    else
    {
        if (b == c)
        {
            temp = blendLinear(bitmaps[a], bitmaps[d], 0);
            HBITMAP result = blendLinear(temp, bitmaps[b], 1);
            DeleteObject(temp);
            return result;
        }
        if (c == d)
        {
            temp = blendLinear(bitmaps[a], bitmaps[b], 1);
            HBITMAP result = blendLinear(temp, bitmaps[c], 0);
            DeleteObject(temp);
            return result;
        }
        HBITMAP left = blendLinear(bitmaps[a], bitmaps[b], 1);
        HBITMAP right = blendLinear(bitmaps[d], bitmaps[c], 1);
        HBITMAP result = blendLinear(left, right, 0);
        DeleteObject(left);
        DeleteObject(right);
        return result;
    }
    HBITMAP result = blendLinear(other, temp, vertical);
    DeleteObject(temp);
    return result;
}

BYTE g_nullPixel[4];

bool DIBPixels::create(int , int width, int height)
{
    if (m_bits)
    {
        ::operator delete[](m_bits);
        m_bits = 0;
    }
    int count = width * height;
    DWORD* bits = new DWORD[count]();
    if (!bits)
        return false;
    m_width = width;
    m_height = height;
    m_bits = bits;
    m_lastRow = bits + m_width * (height - 1);
    return true;
}

bool DIBPixels::load(HBITMAP bitmap)
{
    if (m_bits)
        ::operator delete[](m_bits);
    m_lastRow = 0;
    m_width = 0;
    m_height = 0;
    BITMAPINFO bmi;
    if (getBitmapInfo(bitmap, &bmi))
        return false;
    HDC screen = GetDC(0);
    if (!screen)
        return false;
    int count = MulDiv(bmi.bmiHeader.biSizeImage, 8, bmi.bmiHeader.biBitCount);
    m_bits = new DWORD[count]();
    if (!GetDIBits(screen, bitmap, 0, bmi.bmiHeader.biHeight, m_bits, &bmi, DIB_RGB_COLORS))
    {
        ::operator delete[](m_bits);
        m_bits = 0;
    }
    ReleaseDC(0, screen);
    if (!m_bits)
        return false;
    m_height = bmi.bmiHeader.biHeight;
    m_width = bmi.bmiHeader.biWidth;
    m_lastRow = m_bits + bmi.bmiHeader.biWidth * (bmi.bmiHeader.biHeight - 1);
    return true;
}
