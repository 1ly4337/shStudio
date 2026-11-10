#include "imageio.h"
#include "gdiutil.h"
#include "Bytes.h"
#include "Exception.h"
#include <cstring>
#include <algorithm>
#include <objidl.h>
namespace Gdiplus
{
    using std::max;
    using std::min;
}
#include <gdiplus.h>

#pragma comment(lib, "gdiplus.lib")

BYTE* BmpFile::init(int width, int height, int bitsPerPixel)
{
    if (bitsPerPixel == 32)
        bitsPerPixel = 24;
    DWORD size = height * ((((bitsPerPixel * width) >> 3) + 3) & 0xFFFFFFFC);
    file.bfType = 0x4D42;
    file.bfSize = size + 54;
    file.bfReserved1 = 0;
    file.bfReserved2 = 0;
    file.bfOffBits = 54;
    info.biSize = 40;
    info.biWidth = width;
    info.biHeight = height;
    info.biPlanes = 1;
    info.biBitCount = (WORD)bitsPerPixel;
    info.biCompression = BI_RGB;
    info.biSizeImage = size;
    info.biXPelsPerMeter = 96;
    info.biYPelsPerMeter = 96;
    info.biClrUsed = 0;
    info.biClrImportant = 0;
    bits = (BYTE*)::operator new[](size);
    return bits;
}

int writeGrayBmpHeader(const int* dims, Bytes* out)
{
    aStream& stream = *out;
    stream.wipe();
    int rowBytes = dims[0];
    if (rowBytes & 3)
        rowBytes -= rowBytes % 4;
    BITMAPFILEHEADER file;
    file.bfType = 0x4D42;
    file.bfSize = rowBytes * dims[1] + 1078;
    file.bfReserved1 = 0;
    file.bfReserved2 = 0;
    file.bfOffBits = 1078;
    if (!stream.append(&file, 14))
        throw OutOfMemoryException(14, L"");
    BITMAPINFOHEADER info;
    info.biSize = 40;
    info.biWidth = rowBytes;
    info.biHeight = dims[1];
    info.biPlanes = 1;
    info.biBitCount = 8;
    info.biCompression = BI_RGB;
    info.biSizeImage = rowBytes * dims[1];
    info.biXPelsPerMeter = 96;
    info.biYPelsPerMeter = 96;
    info.biClrUsed = 256;
    info.biClrImportant = 256;
    if (!stream.append(&info, 40))
        throw OutOfMemoryException(40, L"");
    for (int i = 0; i < 256; ++i)
    {
        DWORD entry = (DWORD)i | ((DWORD)i << 8) | ((DWORD)i << 16);
        if (!stream.append(&entry, 4))
            throw OutOfMemoryException(4, L"");
    }
    return 0;
}

DWORD loadImageGdiplus(Bytes& data, HBITMAP* bitmap)
{
    *bitmap = 0;
    aStream& stream = data;
    SIZE_T size = stream.m_size;
    HGLOBAL memory = GlobalAlloc(GMEM_MOVEABLE, size);
    if (!memory)
        return 8;
    void* buffer = GlobalLock(memory);
    if (!buffer)
    {
        DWORD error = GetLastError();
        GlobalFree(memory);
        return error;
    }
    memcpy(buffer, stream.m_data + stream.m_pos, size);
    GlobalUnlock(memory);
    IStream* source = 0;
    HRESULT result = CreateStreamOnHGlobal(memory, FALSE, &source);
    if (!result)
    {
        Gdiplus::Bitmap* image = new Gdiplus::Bitmap(source);
        if (image)
        {
            image->GetHBITMAP(Gdiplus::Color(0xFFFFFFFF), bitmap);
            delete image;
        }
        else
            result = 50;
        source->Release();
    }
    GlobalFree(memory);
    return (DWORD)result;
}

DWORD writeBmp(Bytes& out, HBITMAP bitmap)
{
    static_cast<aStream&>(out).wipe();
    if (!bitmap)
        return 13;
    BITMAP bm;
    if (!GetObjectW(bitmap, sizeof(BITMAP), &bm))
        return GetLastError();
    if (bm.bmBitsPixel < 24)
        return (DWORD)E_NOTIMPL;
    HDC screen = GetDC(0);
    if (!screen)
        return GetLastError();
    HPALETTE oldPalette = SelectPalette(screen, (HPALETTE)GetStockObject(DEFAULT_PALETTE), FALSE);
    RealizePalette(screen);
    BmpFile file;
    file.bits = 0;
    file.init(bm.bmWidth, bm.bmHeight, bm.bmBitsPixel);
    int lines = GetDIBits(screen, bitmap, 0, bm.bmHeight, file.bits, (BITMAPINFO*)&file.info, DIB_RGB_COLORS);
    DWORD result;
    if (!lines || !file.bits)
        result = (DWORD)E_FAIL;
    else
    {
        SelectPalette(screen, oldPalette, FALSE);
        ReleaseDC(0, screen);
        if (lines != bm.bmHeight)
            result = (DWORD)E_FAIL;
        else
        {
            oStream& stream = out;
            stream.put(&file.file, 14);
            stream.put(&file.info, 40);
            stream.put(file.bits, file.info.biSizeImage);
            result = 0;
        }
    }
    if (file.bits)
        ::operator delete[](file.bits);
    return result;
}

int DdsHeader::getFormat()
{
    int format;
    DWORD size;
    DWORD pf = pfFlags;
    if (!(pf & 4))
    {
        if (pf & 0x20000)
            format = ((pf & 1) != 0) + 9;
        else
            format = (pf & 1) == 0;
        size = height * width * depth * (rgbBitCount >> 3);
    }
    else
    {
        DWORD blocks = depth * (width + 3) / 4 * ((height + 3) / 4);
        DWORD pixels = width * height * depth;
        switch (fourCC)
        {
        case 0x31545844:
            format = 2;
            size = 8 * blocks;
            break;
        case 0x32545844:
            format = 3;
            size = 16 * blocks;
            break;
        case 0x33545844:
            format = 4;
            size = 16 * blocks;
            break;
        case 0x34545844:
            format = 5;
            size = 16 * blocks;
            break;
        case 0x35545844:
            format = 6;
            size = 16 * blocks;
            break;
        case 0x32495441:
            format = 7;
            size = 16 * blocks;
            break;
        case 0x31495441:
            format = 8;
            size = 8 * blocks;
            break;
        case 0x42475852:
            format = 11;
            size = 16 * blocks;
            break;
        case 36:
            format = 12;
            size = 8 * pixels;
            break;
        case 111:
            format = 13;
            size = 2 * pixels;
            break;
        case 112:
            format = 14;
            size = 4 * pixels;
            break;
        case 113:
            format = 15;
            size = 8 * pixels;
            break;
        case 114:
            format = 16;
            size = 4 * pixels;
            break;
        case 115:
            format = 17;
            size = 8 * pixels;
            break;
        case 116:
            format = 18;
            size = 16 * pixels;
            break;
        default:
            format = 255;
            size = 16 * blocks;
            break;
        }
    }
    if ((flags & 0x80008) == 0 || !pitchOrLinearSize)
    {
        flags |= 0x80000;
        pitchOrLinearSize = size;
    }
    return format;
}

void DdsHeader::adjustSize(int format)
{
    if ((int)depth <= 1)
        return;
    if (!(caps2 & 0x200000))
        depth = 1;
    switch (format)
    {
    case 0:
    case 1:
    case 9:
    case 10:
    {
        int h = (int)height;
        int w = (int)width;
        if (h < 1)
            h = 1;
        if (w < 1)
            w = 1;
        pitchOrLinearSize = (rgbBitCount >> 3) * w * h;
        break;
    }
    case 2:
    case 8:
        pitchOrLinearSize = 8 * (height + 3) / 4 * ((width + 3) / 4);
        break;
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 11:
        pitchOrLinearSize = 16 * (height + 3) / 4 * ((width + 3) / 4);
        break;
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    {
        int h = (int)height;
        int w = (int)width;
        BYTE bpp = bytesPerPixel(format);
        if (h < 1)
            h = 1;
        if (w < 1)
            w = 1;
        pitchOrLinearSize = w * (h * bpp);
        break;
    }
    default:
        break;
    }
    flags |= 0x80000;
    pitchOrLinearSize *= depth;
}

BYTE DdsHeader::bytesPerPixel(int format) const
{
    switch (format)
    {
    case 9:
    case 10:
    case 0:
        return (BYTE)(rgbBitCount >> 3);
    case 1:
    case 7:
    case 11:
        return 3;
    case 8:
        return 1;
    case 12:
    case 15:
    case 17:
        return 8;
    default:
        return format == 18 ? 16 : 4;
    }
}

BYTE* DdsHeader::readData(iStream& in)
{
    aStream& stream = in;
    if (flags & 0x80000)
    {
        if ((DWORD)(stream.m_size - stream.m_pos) < pitchOrLinearSize)
            return 0;
        BYTE* data = (BYTE*)::operator new[](pitchOrLinearSize);
        if (data)
            in.read(data, pitchOrLinearSize);
        return data;
    }
    DWORD rowBytes = (width * rgbBitCount) >> 3;
    if (stream.m_size - stream.m_pos < (int)(height * rowBytes * depth))
        return 0;
    BYTE* data = (BYTE*)::operator new[](height * rowBytes * depth);
    if (!data)
        return data;
    BYTE* p = data;
    for (int z = 0; z < (int)depth; ++z)
    {
        for (int y = 0; y < (int)height; ++y)
        {
            in.read(p, rowBytes);
            p += rowBytes;
        }
    }
    return data;
}

BYTE DdsImage::glTypeSize(DWORD glType)
{
    switch (glType)
    {
    case 0x1400:
    case 0x1401:
        return 1;
    case 0x1402:
    case 0x1403:
    case 0x140B:
        return 2;
    case 0x1404:
    case 0x1405:
    case 0x1406:
        return 4;
    case 0x140A:
        return 8;
    default:
        return 0;
    }
}

BYTE DdsImage::componentCount(int format)
{
    if (format != 1 && format != 7 && format != 11)
    {
        if (format == 9 || format == 8)
            return 1;
        if (format == 10)
            return 2;
        if (format != 14 && format != 17 && format != 16 && format != 13)
            return 4;
    }
    return 3;
}

DWORD DdsImage::glFormatFor(BYTE components)
{
    switch (components)
    {
    case 1:
        return 0x1909;
    case 2:
        return 0x190A;
    case 3:
        return 0x1907;
    case 4:
        return 0x1908;
    default:
        return 0;
    }
}

bool DdsImage::create(DdsHeader* header, BYTE count, DWORD format, DWORD type, const void* src)
{
    memset(this, 0, 0x78);
    if (!count || count > 4)
        return false;
    BYTE size = glTypeSize(type);
    if (!size)
        return false;
    if (!header->width)
        header->width = 1;
    if (!header->height)
        header->height = 1;
    if (!header->depth)
        header->depth = 1;
    width = header->width;
    height = header->height;
    depth = header->depth;
    rowSize = width * count * size;
    sliceSize = rowSize * height;
    components = count;
    componentSize = size;
    dataSize = sliceSize * depth;
    glFormat = format;
    glType = type;
    field28 = 0x601;
    field34 = 0x400;
    field6C = 0x70B;
    field68 = 0;
    data = (BYTE*)::operator new[](dataSize);
    if (!data)
        return false;
    if (src)
        memcpy(data, src, dataSize);
    return true;
}

bool DdsImage::init(DdsHeader* header, int format, const void* src)
{
    BYTE count = 4;
    field28 = 0x602;
    switch (format)
    {
    case 0:
        return create(header, 4, 0x1908, 2 * (packed10 != 0) + 0x1401, 0);
    case 1:
        return create(header, 4, 0x1907, 0x1401, 0);
    case 7:
        return create(header, 3, 0x1907, 0x1401, 0);
    case 8:
        return create(header, 1, 0x1909, 0x1401, 0);
    case 9:
        if (header->rgbBitCount == 16 && header->rMask == 0xFFFF)
            return create(header, 1, 0x1909, 0x1403, 0);
        return create(header, 1, 0x1909, 0x1401, 0);
    case 10:
        return create(header, 2, 0x190A, 0x1401, 0);
    case 12:
    {
        BYTE n = componentCount(12);
        return create(header, n, glFormatFor(n), 0x1403, 0);
    }
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    {
        BYTE n = componentCount(format);
        return create(header, n, glFormatFor(n), 0x1406, 0);
    }
    case 254:
        return create(header, 4, 0x1908, 0x1401, src);
    default:
    {
        DWORD glFormat = 0x1908;
        if (format == 11)
        {
            count = 3;
            glFormat = 0x1907;
        }
        return create(header, count, glFormat, 0x1401, 0) != 0;
    }
    }
}

static void maskShift(DWORD mask, DWORD& shift, DWORD& bits)
{
    if (!mask)
    {
        bits = 0;
        shift = 0;
        return;
    }
    DWORD i;
    for (i = 0; i < 32; ++i)
    {
        if (mask & 1)
            break;
        mask >>= 1;
    }
    shift = i;
    DWORD j;
    for (j = 0; j < 8; ++j)
    {
        if (!(mask & 1))
            break;
        mask >>= 1;
    }
    bits = 8 - j;
}

static inline BYTE channel(DWORD value, DWORD mask, DWORD shift, DWORD bits)
{
    return (BYTE)((BYTE)((value & mask) >> shift) << bits);
}

bool DdsImage::convertMasked(const DdsHeader* header, const BYTE* src, int format)
{
    if (format == 9 && header->rgbBitCount == 16 && header->rMask == 0xFFFF)
    {
        memcpy(data, src, dataSize);
        return true;
    }
    DWORD rShift, rBits, gShift, gBits, bShift, bBits, aShift, aBits;
    maskShift(header->rMask, rShift, rBits);
    maskShift(header->gMask, gShift, gBits);
    maskShift(header->bMask, bShift, bBits);
    maskShift(header->aMask, aShift, aBits);
    DWORD bpp = header->rgbBitCount >> 3;
    DWORD value = 0;
    for (DWORD pos = 0; pos < dataSize; pos += components)
    {
        if (dataSize - pos >= 4)
            value = src[0] | (src[1] << 8) | (src[2] << 16) | ((DWORD)src[3] << 24);
        else if (bpp == 3)
            value = src[0] | (src[1] << 8) | (src[2] << 16);
        else if (bpp == 1)
            value = src[0];
        else if (bpp == 2)
            value = src[0] | (src[1] << 8);
        src += bpp;
        data[pos] = channel(value, header->bMask, bShift, bBits);
        if (components >= 3)
        {
            data[pos + 1] = channel(value, header->gMask, gShift, gBits);
            data[pos + 2] = channel(value, header->rMask, rShift, rBits);
            if (components == 4)
            {
                data[pos + 3] = channel(value, header->aMask, aShift, aBits);
                if (aBits >= 7)
                    data[pos + 3] = data[pos + 3] ? 0xFF : 0;
                else if (aBits >= 4)
                    data[pos + 3] |= data[pos + 3] >> 4;
            }
        }
        else if (components == 2)
        {
            data[pos + 1] = channel(value, header->aMask, aShift, aBits);
            if (aBits >= 7)
                data[pos + 1] = data[pos + 1] ? 0xFF : 0;
            else if (aBits >= 4)
                data[pos + 1] |= data[pos + 3] >> 4;
        }
    }
    return true;
}

static inline void expandColors(BYTE* palette, BYTE lo0, BYTE hi0, BYTE lo1, BYTE hi1)
{
    BYTE g0 = (BYTE)((lo0 >> 5) | ((hi0 & 7) << 3));
    BYTE g1 = (BYTE)((lo1 >> 5) | ((hi1 & 7) << 3));
    palette[0] = (BYTE)((lo0 & 0x1F) << 3) | (BYTE)((lo0 & 0x1F) >> 2);
    palette[1] = (BYTE)(g0 << 2) | (BYTE)(g0 >> 3);
    palette[2] = (BYTE)((hi0 >> 3) << 3) | (BYTE)(hi0 >> 5);
    palette[4] = (BYTE)((lo1 & 0x1F) << 3) | (BYTE)((lo1 & 0x1F) >> 2);
    palette[5] = (BYTE)(g1 << 2) | (BYTE)(g1 >> 3);
    palette[6] = (BYTE)((hi1 >> 3) << 3) | (BYTE)(hi1 >> 5);
    palette[8] = (BYTE)((palette[4] + 2 * palette[0] + 1) / 3);
    palette[9] = (BYTE)((palette[5] + 2 * palette[1] + 1) / 3);
    palette[10] = (BYTE)((palette[6] + 2 * palette[2] + 1) / 3);
    palette[12] = (BYTE)((palette[0] + 2 * palette[4] + 1) / 3);
    palette[13] = (BYTE)((palette[1] + 2 * palette[5] + 1) / 3);
    palette[14] = (BYTE)((palette[2] + 2 * palette[6] + 1) / 3);
}

bool DdsImage::decodeDXT1(const WORD* src)
{
    BYTE palette[16];
    palette[3] = 0xFF;
    palette[7] = 0xFF;
    palette[11] = 0xFF;
    for (DWORD z = 0; z < depth; ++z)
    {
        if (!height)
            continue;
        DWORD y = 0;
        do
        {
            if (width)
            {
                DWORD x = 2;
                DWORD current;
                do
                {
                    WORD c0 = src[0];
                    WORD c1 = src[1];
                    DWORD bits = *(const DWORD*)(src + 2);
                    src += 4;
                    BYTE r0 = (BYTE)((c0 >> 11) & 0x1F);
                    BYTE r1 = (BYTE)((c1 >> 11) & 0x1F);
                    BYTE g0 = (BYTE)((c0 >> 5) & 0x3F);
                    BYTE g1 = (BYTE)((c1 >> 5) & 0x3F);
                    palette[2] = (BYTE)(r0 << 3) | (BYTE)(r0 >> 2);
                    palette[1] = (BYTE)(g0 << 2) | (BYTE)(g0 >> 3);
                    palette[0] = (BYTE)(c0 << 3) | (BYTE)(r0 >> 2);
                    palette[6] = (BYTE)(r1 << 3) | (BYTE)(r1 >> 2);
                    palette[5] = (BYTE)(g1 << 2) | (BYTE)(g1 >> 3);
                    palette[4] = (BYTE)(c1 << 3) | (BYTE)(r1 >> 2);
                    if (c0 <= c1)
                    {
                        palette[10] = (BYTE)((palette[2] + palette[6]) / 2);
                        palette[9] = (BYTE)((palette[1] + palette[5]) / 2);
                        palette[8] = (BYTE)((palette[0] + palette[4]) / 2);
                        palette[15] = 0;
                    }
                    else
                    {
                        palette[10] = (BYTE)((palette[6] + 2 * palette[2] + 1) / 3);
                        palette[9] = (BYTE)((palette[5] + 2 * palette[1] + 1) / 3);
                        palette[8] = (BYTE)((palette[4] + 2 * palette[0] + 1) / 3);
                        palette[15] = 0xFF;
                    }
                    palette[14] = (BYTE)((palette[2] + 2 * palette[6] + 1) / 3);
                    palette[13] = (BYTE)((palette[1] + 2 * palette[5] + 1) / 3);
                    palette[12] = (BYTE)((palette[0] + 2 * palette[4] + 1) / 3);
                    DWORD row = y;
                    for (int shift = 4; shift < 36; shift += 8, ++row)
                    {
                        for (int k = 0; k < 4; ++k)
                        {
                            const BYTE* entry = &palette[4 * ((bits >> (shift - 4 + 2 * k)) & 3)];
                            DWORD px = x - 2 + k;
                            if (px < width && row < height)
                            {
                                DWORD offset = z * sliceSize + row * rowSize + px * components;
                                data[offset] = entry[0];
                                data[offset + 1] = entry[1];
                                data[offset + 2] = entry[2];
                                data[offset + 3] = entry[3];
                            }
                        }
                    }
                    current = x;
                    x += 4;
                } while (current + 2 < width);
            }
            y += 4;
        } while (y < height);
    }
    return true;
}

bool DdsImage::decodeDXT3(const BYTE* src)
{
    BYTE palette[16];
    for (DWORD z = 0; z < depth; ++z)
    {
        if (!height)
            continue;
        DWORD y = 0;
        do
        {
            if (width)
            {
                DWORD x = 2;
                DWORD current;
                do
                {
                    const BYTE* block = src;
                    expandColors(palette, src[8], src[9], src[10], src[11]);
                    DWORD bits = *(const DWORD*)(src + 12);
                    src += 16;
                    DWORD row = y;
                    for (int shift = 4; shift < 36; shift += 8, ++row)
                    {
                        for (int k = 0; k < 4; ++k)
                        {
                            const BYTE* entry = &palette[4 * ((bits >> (shift - 4 + 2 * k)) & 3)];
                            DWORD px = x - 2 + k;
                            if (px < width && row < height)
                            {
                                DWORD offset = row * rowSize + z * sliceSize + px * components;
                                data[offset] = entry[0];
                                data[offset + 1] = entry[1];
                                data[offset + 2] = entry[2];
                            }
                        }
                    }
                    row = y;
                    for (int r = 0; r < 4; ++r, ++row)
                    {
                        DWORD word = block[2 * r] + (block[2 * r + 1] << 8);
                        BYTE alpha[4];
                        alpha[0] = block[2 * r] & 0xF;
                        alpha[1] = (BYTE)((word >> 4) & 0xF);
                        alpha[2] = (BYTE)((word >> 8) & 0xF);
                        alpha[3] = (BYTE)(word >> 12);
                        for (int k = 0; k < 4; ++k)
                        {
                            DWORD px = x - 2 + k;
                            if (px < width && row < height)
                            {
                                BYTE* p = &data[row * rowSize + px * components + z * sliceSize + 3];
                                *p = alpha[k];
                                *p |= (BYTE)(16 * *p);
                            }
                        }
                    }
                    current = x;
                    x += 4;
                } while (current + 2 < width);
            }
            y += 4;
        } while (y < height);
    }
    return true;
}

bool DdsImage::decodeDXT5(const BYTE* src)
{
    BYTE palette[16];
    BYTE alpha[8];
    for (DWORD z = 0; z < depth; ++z)
    {
        if (!height)
            continue;
        DWORD y = 0;
        do
        {
            if (width)
            {
                DWORD x = 2;
                do
                {
                    if (y >= height || x - 2 >= width)
                        break;
                    BYTE a0 = src[0];
                    BYTE a1 = src[1];
                    alpha[0] = a0;
                    alpha[1] = a1;
                    const BYTE* alphaBits = src + 2;
                    expandColors(palette, src[8], src[9], src[10], src[11]);
                    DWORD bits = *(const DWORD*)(src + 12);
                    src += 16;
                    DWORD row = y;
                    for (int shift = 4; shift < 36; shift += 8, ++row)
                    {
                        for (int k = 0; k < 4; ++k)
                        {
                            const BYTE* entry = &palette[4 * ((bits >> (shift - 4 + 2 * k)) & 3)];
                            DWORD px = x - 2 + k;
                            if (px < width && row < height)
                            {
                                DWORD offset = z * sliceSize + row * rowSize + px * components;
                                data[offset] = entry[0];
                                data[offset + 1] = entry[1];
                                data[offset + 2] = entry[2];
                            }
                        }
                    }
                    if (a0 <= a1)
                    {
                        alpha[2] = (BYTE)((a1 + 4 * a0 + 2) / 5);
                        alpha[6] = 0;
                        alpha[7] = 0xFF;
                        alpha[3] = (BYTE)((a0 + 2 * (a0 + 1) + 2 * a1) / 5);
                        alpha[4] = (BYTE)((a1 + 2 * (a1 + 1) + 2 * a0) / 5);
                        alpha[5] = (BYTE)((a0 + 4 * a1 + 2) / 5);
                    }
                    else
                    {
                        alpha[2] = (BYTE)((a1 + 6 * a0 + 3) / 7);
                        alpha[3] = (BYTE)((a0 + 4 * a0 + 3 + 2 * a1) / 7);
                        alpha[4] = (BYTE)((3 * (a1 + 1) + 4 * a0) / 7);
                        alpha[5] = (BYTE)((3 * (a0 + 1) + 4 * a1) / 7);
                        alpha[6] = (BYTE)((a1 + 4 * a1 + 3 + 2 * a0) / 7);
                        alpha[7] = (BYTE)((a0 + 6 * a1 + 3) / 7);
                    }
                    for (int half = 0; half < 2; ++half)
                    {
                        const BYTE* b = alphaBits + 3 * half;
                        DWORD indexes = b[0] | (*(const WORD*)(b + 1) << 8);
                        row = y + 2 * half;
                        for (int r = 0; r < 2; ++r, ++row)
                        {
                            for (int k = 0; k < 4; ++k)
                            {
                                DWORD px = x - 2 + k;
                                if (px < width && row < height)
                                    data[z * sliceSize + row * rowSize + px * components + 3] = alpha[indexes & 7];
                                indexes >>= 3;
                            }
                        }
                    }
                    x += 4;
                } while (x - 2 < width);
            }
            y += 4;
        } while (y < height);
    }
    return true;
}

bool DdsImage::decode(const DdsHeader* header, const BYTE* src, int format)
{
    switch (format)
    {
    case 0:
    case 1:
    case 9:
    case 10:
        if (packed10)
            return false;
        return convertMasked(header, src, format);
    case 2:
        return decodeDXT1((const WORD*)src);
    case 3:
        if (!decodeDXT3(src))
            return false;
        unpremultiply();
        return true;
    case 4:
        return decodeDXT3(src);
    case 5:
        if (!decodeDXT5(src))
            return false;
        unpremultiply();
        return true;
    case 6:
        return decodeDXT5(src);
    case 12:
        memcpy(data, src, dataSize);
        return true;
    case 254:
        return true;
    default:
        return false;
    }
}

void DdsImage::unpremultiply()
{
    for (DWORD i = 0; i < dataSize; i += 4)
    {
        BYTE* p = data + i;
        unsigned a = p[3];
        if (a)
        {
            p[0] = (BYTE)(((unsigned)p[0] << 8) / a);
            p[1] = (BYTE)(((unsigned)p[1] << 8) / a);
            p[2] = (BYTE)(((unsigned)p[2] << 8) / a);
        }
    }
}

DWORD DdsImage::toBitmap(HGDIOBJ* bitmap)
{
    DWORD error = 0;
    *bitmap = 0;
    BITMAPINFO bmi;
    memset(&bmi, 0, sizeof(bmi));
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biSize = 44;
    bmi.bmiHeader.biHeight = height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = height * (((DWORD)(32 * bmi.bmiHeader.biWidth + 31) >> 3) & 0x1FFFFFFC);
    bmi.bmiHeader.biXPelsPerMeter = 96;
    bmi.bmiHeader.biYPelsPerMeter = 96;
    bmi.bmiHeader.biClrUsed = 0;
    bmi.bmiHeader.biClrImportant = 0;
    HDC screen = GetDC(0);
    if (!screen)
        return GetLastError();
    HBITMAP result = CreateCompatibleBitmap(screen, width, height);
    *bitmap = result;
    if (result)
    {
        bmi.bmiHeader.biHeight = -bmi.bmiHeader.biHeight;
        if (!SetDIBits(screen, result, 0, height, data, &bmi, DIB_RGB_COLORS))
        {
            error = GetLastError();
            DeleteObject(*bitmap);
            *bitmap = 0;
        }
    }
    else
        error = GetLastError();
    ReleaseDC(0, screen);
    return error;
}

DWORD loadDDS(Bytes& data, HGDIOBJ* bitmap)
{
    *bitmap = 0;
    Bytes in(data);
    DWORD result;
    try
    {
        DdsHeader header;
        in.read(&header, 128);
        if (!header.depth)
            header.depth = 1;
        if (header.magic != 0x20534444)
            throw 11;
        if (header.size != 124 && header.size != 0x20534444)
            throw 11;
        if (header.pfSize != 32)
            throw 11;
        if (!header.width || !header.height)
            throw 11;
        int format = header.getFormat();
        if (format == 255)
            throw 11;
        bool packed = false;
        if (header.rMask == 0x3FF00000)
        {
            if (header.gMask == 0xFFC00 && header.bMask == 0x3FF && header.aMask == 0xC0000000)
                packed = true;
        }
        else if (header.rMask == 0x3FF && header.gMask == 0xFFC00 && header.bMask == 0x3FF00000
                 && header.aMask == 0xC0000000)
            packed = true;
        if ((header.caps & 8) && (header.caps2 & 0x200))
            throw 50;
        DdsImage image;
        memset(&image, 0, sizeof(image));
        image.packed10 = packed;
        header.adjustSize(format);
        BYTE* raw = header.readData(in);
        if (!raw)
            throw 11;
        if (!image.init(&header, format, raw))
            throw 11;
        if (!image.decode(&header, raw, format))
            throw 11;
        ::operator delete[](raw);
        result = image.toBitmap(bitmap);
        if (!result && !*bitmap)
            result = (DWORD)E_FAIL;
    }
    catch (int code)
    {
        result = (DWORD)code;
    }
    catch (...)
    {
        result = (DWORD)E_FAIL;
    }
    return result;
}

bool tgaGetInfo(const BYTE* header, TgaInfo* info)
{
    info->width = *(const short*)(header + 12);
    info->height = *(const short*)(header + 14);
    switch (header[2])
    {
    case 1:
    case 2:
    case 9:
    case 10:
        if (header[16] > 24)
            info->components = info->bytesPerPixel = 4;
        else
            info->components = info->bytesPerPixel = 3;
        return true;
    case 3:
    case 11:
        if (header[16] == 8)
            info->components = info->bytesPerPixel = 1;
        else
            info->components = info->bytesPerPixel = 2;
        return true;
    default:
        return false;
    }
}

void tgaRead16(iStream& in, const TgaInfo* info, BYTE* pixels)
{
    for (int y = 0; y < info->height; ++y)
    {
        for (int x = 0; x < info->width; ++x)
        {
            WORD v = (WORD)in.readShort();
            BYTE* p = pixels + 4 * (x + y * info->width);
            p[0] = (BYTE)(8 * v);
            p[2] = (BYTE)((v >> 7) & 0xF8);
            p[1] = (BYTE)((v >> 2) & 0xF8);
            p[3] = 0;
        }
    }
}

void tgaRead24(iStream& in, const TgaInfo* info, BYTE* pixels)
{
    BYTE* p = pixels;
    for (int i = 0; i < info->height * info->width; ++i, p += 4)
    {
        p[0] = (BYTE)in.readByte();
        p[1] = (BYTE)in.readByte();
        p[2] = (BYTE)in.readByte();
        p[3] = 0;
    }
}

void tgaRead32(iStream& in, const TgaInfo* info, BYTE* pixels)
{
    BYTE* p = pixels;
    for (int i = 0; i < info->height * info->width; ++i, p += 4)
    {
        p[0] = (BYTE)in.readByte();
        p[1] = (BYTE)in.readByte();
        p[2] = (BYTE)in.readByte();
        p[3] = (BYTE)in.readByte();
        double a = (double)p[3] / 255.0;
        p[2] = (BYTE)(int)((double)p[2] * a);
        p[1] = (BYTE)(int)((double)p[1] * a);
        p[0] = (BYTE)(int)((double)p[0] * a);
    }
}

void tgaReadRle24(iStream& in, const TgaInfo* info, BYTE* pixels)
{
    BYTE* p = pixels;
    BYTE* end = pixels + 4 * info->width * info->height;
    bool fourComponents = info->bytesPerPixel == 4;
    while (p < end)
    {
        BYTE packet = (BYTE)in.readByte();
        int count = (packet & 0x7F) + 1;
        if (!(packet & 0x80))
        {
            for (; count > 0; --count, p += 4)
            {
                p[0] = (BYTE)in.readByte();
                p[1] = (BYTE)in.readByte();
                p[2] = (BYTE)in.readByte();
            }
        }
        else
        {
            BYTE b = (BYTE)in.readByte();
            BYTE g = (BYTE)in.readByte();
            BYTE r = (BYTE)in.readByte();
            DWORD pixel = b | ((g | ((DWORD)r << 8)) << 8);
            for (; count > 0; --count, p += 4)
            {
                *(DWORD*)p = pixel;
                if (fourComponents)
                {
                    BYTE a = p[3];
                    if (a != 0xFF)
                    {
                        double f = (double)a / 255.0;
                        p[2] = (BYTE)(int)((double)p[2] * f);
                        p[1] = (BYTE)(int)((double)p[1] * f);
                        p[0] = (BYTE)(int)((double)p[0] * f);
                    }
                }
            }
        }
    }
}

void tgaReadRle32(iStream& in, const TgaInfo* info, BYTE* pixels)
{
    BYTE* p = pixels;
    BYTE* end = pixels + 4 * info->width * info->height;
    while (p < end)
    {
        BYTE packet = (BYTE)in.readByte();
        int count = (packet & 0x7F) + 1;
        if (!(packet & 0x80))
        {
            for (; count > 0; --count, p += 4)
            {
                p[0] = (BYTE)in.readByte();
                p[1] = (BYTE)in.readByte();
                p[2] = (BYTE)in.readByte();
                p[3] = (BYTE)in.readByte();
                double a = (double)p[3] / 255.0;
                p[2] = (BYTE)(int)((double)p[2] * a);
                p[1] = (BYTE)(int)((double)p[1] * a);
                p[0] = (BYTE)(int)((double)p[0] * a);
            }
        }
        else
        {
            DWORD pixel = in.readInt();
            for (; count > 0; --count, p += 4)
                *(DWORD*)p = pixel;
        }
    }
}

DWORD loadTGA(Bytes& data, HGDIOBJ* bitmap)
{
    *bitmap = 0;
    aStream& source = data;
    if ((unsigned)(source.m_size - source.m_pos) < 18)
        return 11;
    Bytes in(data);
    BYTE header[18];
    in.read(header, 18);
    TgaInfo info;
    if (header[1] || !tgaGetInfo(header, &info))
        return 50;
    BYTE type = header[2];
    if ((type & 7) != 2)
        return 50;
    BYTE bpp = header[16];
    if (bpp < 24)
        return 50;
    DWORD result;
    try
    {
        in.skip(header[0]);
        int count = info.height * info.width;
        BYTE* pixels = (BYTE*)new DWORD[count]();
        if (!pixels)
            return 8;
        if (type == 2)
        {
            switch (bpp)
            {
            case 16:
                tgaRead16(in, &info, pixels);
                break;
            case 24:
                tgaRead24(in, &info, pixels);
                break;
            case 32:
                tgaRead32(in, &info, pixels);
                break;
            }
        }
        else
        {
            if (type != 10)
            {
                delete[] (DWORD*)pixels;
                return 50;
            }
            if (bpp == 24)
                tgaReadRle24(in, &info, pixels);
            else if (bpp == 32)
                tgaReadRle32(in, &info, pixels);
        }
        HDC screen = GetDC(0);
        if (screen)
        {
            HBITMAP created = CreateCompatibleBitmap(screen, info.width, info.height);
            *bitmap = created;
            if (created)
            {
                BITMAPINFO bmi;
                result = getBitmapInfo(created, &bmi);
                if (!result && !SetDIBits(screen, (HBITMAP)*bitmap, 0, info.height, pixels, &bmi, DIB_RGB_COLORS))
                {
                    result = GetLastError();
                    DeleteObject(*bitmap);
                    *bitmap = 0;
                }
            }
            else
                result = GetLastError();
            ReleaseDC(0, screen);
            delete[] (DWORD*)pixels;
        }
        else
        {
            result = GetLastError();
            delete[] (DWORD*)pixels;
        }
    }
    catch (...)
    {
        result = 11;
    }
    return result;
}
