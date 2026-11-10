#pragma once
#include <windows.h>

class Bytes;
class iStream;

struct BmpFile
{
    BITMAPFILEHEADER file;
    BITMAPINFOHEADER info;
    BYTE*            bits;

    BYTE* init(int width, int height, int bitsPerPixel);
};
static_assert(sizeof(BmpFile) == 0x3C, "BmpFile size");

int   writeGrayBmpHeader(const int* dims, Bytes* out);
DWORD loadImageGdiplus(Bytes& data, HBITMAP* bitmap);
DWORD writeBmp(Bytes& out, HBITMAP bitmap);

struct DdsHeader
{
    DWORD magic;
    DWORD size;
    DWORD flags;
    DWORD height;
    DWORD width;
    DWORD pitchOrLinearSize;
    DWORD depth;
    DWORD mipMapCount;
    DWORD reserved1[11];
    DWORD pfSize;
    DWORD pfFlags;
    DWORD fourCC;
    DWORD rgbBitCount;
    DWORD rMask;
    DWORD gMask;
    DWORD bMask;
    DWORD aMask;
    DWORD caps;
    DWORD caps2;
    DWORD caps3;
    DWORD caps4;
    DWORD reserved2;

    int   getFormat();
    void  adjustSize(int format);
    BYTE  bytesPerPixel(int format) const;
    BYTE* readData(iStream& in);
};
static_assert(sizeof(DdsHeader) == 0x80, "DdsHeader size");

struct DdsImage
{
    DWORD width;
    DWORD height;
    DWORD depth;
    BYTE  components;
    BYTE  componentSize;
    DWORD rowSize;
    BYTE* data;
    DWORD dataSize;
    DWORD sliceSize;
    DWORD glFormat;
    DWORD glType;
    DWORD field28;
    DWORD field2C[2];
    DWORD field34;
    DWORD field38[12];
    DWORD field68;
    DWORD field6C;
    DWORD field70;
    bool  packed10;

    ~DdsImage()
    {
        if (data)
            ::operator delete[](data);
    }

    static BYTE glTypeSize(DWORD glType);
    static BYTE componentCount(int format);
    static DWORD glFormatFor(BYTE components);

    bool create(DdsHeader* header, BYTE components, DWORD glFormat, DWORD glType, const void* src);
    bool init(DdsHeader* header, int format, const void* src);
    bool convertMasked(const DdsHeader* header, const BYTE* src, int format);
    bool decodeDXT1(const WORD* src);
    bool decodeDXT3(const BYTE* src);
    bool decodeDXT5(const BYTE* src);
    bool decode(const DdsHeader* header, const BYTE* src, int format);
    void unpremultiply();
    DWORD toBitmap(HGDIOBJ* bitmap);
};
static_assert(sizeof(DdsImage) == 0x78, "DdsImage size");

DWORD loadDDS(Bytes& data, HGDIOBJ* bitmap);

struct TgaInfo
{
    short width;
    short height;
    BYTE  components;
    BYTE  bytesPerPixel;
};
static_assert(sizeof(TgaInfo) == 6, "TgaInfo size");

bool  tgaGetInfo(const BYTE* header, TgaInfo* info);
void  tgaRead16(iStream& in, const TgaInfo* info, BYTE* pixels);
void  tgaRead24(iStream& in, const TgaInfo* info, BYTE* pixels);
void  tgaRead32(iStream& in, const TgaInfo* info, BYTE* pixels);
void  tgaReadRle24(iStream& in, const TgaInfo* info, BYTE* pixels);
void  tgaReadRle32(iStream& in, const TgaInfo* info, BYTE* pixels);
DWORD loadTGA(Bytes& data, HGDIOBJ* bitmap);
