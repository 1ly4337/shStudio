#include "ImageView.h"
#include "ViewFilters.h"
#include "FileTree.h"
#include "aStream.h"
#include "FileDialogs.h"
#include "p10_vcall.h"
#include "Application.h"
#include "clipboard_A.h"
#include "_imports_p10.h"
#include <gdiplus.h>
#include <shlwapi.h>

COMDLG_FILTERSPEC g_bmpFileFilter[1] = { { L"Bitmap File", L"*.bmp" } };
COMDLG_FILTERSPEC g_ddsFileFilters[2] = { { L"DirectDraw Surface File", L"*.dds" }, { L"Bitmap File", L"*.bmp" } };
COMDLG_FILTERSPEC g_tgaFileFilters[2] = { { L"Targa File", L"*.tga" }, { L"Bitmap File", L"*.bmp" } };

static inline String& ftPath(FileTree* f) { return FIELD(String, f, 0x10); }
#define FRAME_READ(o, b, s, dec) P10_VCALL((o), 0x14C, HRESULT, Bytes*, const String*, bool)((o), (b), (s), (dec))

static const int kKindTga = 0x2E746761;
static const int kKindDds = 0x2E646473;
static const int kKindBmp = 0x2E626D70;

HBITMAP loadBitmapFromMemory(const void* data, size_t size)
{
    HBITMAP bitmap = 0;
    if (!data || !size)
        return 0;
    HGLOBAL memory = GlobalAlloc(GMEM_MOVEABLE, size);
    if (!memory)
        return 0;
    void* p = GlobalLock(memory);
    if (p)
    {
        memcpy(p, data, size);
        GlobalUnlock(memory);
        IStream* stream = 0;
        if (!CreateStreamOnHGlobal(memory, FALSE, &stream))
        {
            Gdiplus::Bitmap* image = Gdiplus::Bitmap::FromStream(stream);
            if (image)
            {
                image->GetHBITMAP(Gdiplus::Color(0xFFFFFF), &bitmap);
                delete image;
            }
            stream->Release();
        }
    }
    GlobalFree(memory);
    return bitmap;
}

bool ImageView::onCreate()
{
    if (!Popup::onCreate())
        return false;
    FileTree* data = m_data;
    if (data && (P10_VCALL(data, 0x08, int)(data) & 2) != 0)
        setModified(true);
    m_modified = true;
    setModified(false);
    String path(ftPath(m_data));
    bool result;
    if (FRAME_READ(g_mainFrame, &m_bytes, &path, false)
        && (!path.endsWithNoCase(L".tga")
            || (path.removeExtension(), path << L".dds", FRAME_READ(g_mainFrame, &m_bytes, &path, false))))
    {
        result = false;
    }
    else
    {
        HGDIOBJ bitmap = 0;
        String ext(PathFindExtensionW(path.m_buffer), (unsigned)-1);
        ext.toLowerInPlace();
        if (ext == L".tga")
        {
            m_field58 = kKindTga;
            loadTGA(m_bytes, &bitmap);
        }
        else if (ext == L".dds")
        {
            m_field58 = kKindDds;
            loadDDS(m_bytes, &bitmap);
        }
        else if (ext == L".bmp")
        {
            m_field58 = kKindBmp;
            loadImageGdiplus(m_bytes, (HBITMAP*)&bitmap);
        }
        else
        {
            aStream& stream = static_cast<aStream&>(static_cast<iStream&>(m_bytes));
            bitmap = loadBitmapFromMemory(stream.m_data + stream.m_pos, (size_t)stream.m_size);
        }
        if (bitmap)
        {
            BITMAP bm;
            GetObjectW(bitmap, sizeof(BITMAP), &bm);
            RECT rc;
            SetRect(&rc, 0, 0, bm.bmWidth, bm.bmHeight);
            DWORD exStyle = getExStyle();
            DWORD style = getStyle();
            AdjustWindowRectEx(&rc, style, TRUE, exStyle);
            int width = rc.right - rc.left;
            int height = rc.bottom - rc.top;
            SetWindowPos(m_hWnd, 0, 0, 0, width, height + m_statusHeight, 0x26u);
            m_bitmap.m_bitmapSize.cx = bm.bmWidth;
            m_bitmap.m_bitmapSize.cy = bm.bmHeight;
            RECT child;
            SetRect(&child, 0, 0, bm.bmWidth, bm.bmHeight);
            m_bitmap.Window::create(m_hWnd, (HMENU)0x65, &child, WS_CHILD | WS_VISIBLE, 0, 0);
            m_bitmap.setBitmap((HBITMAP)bitmap);
            result = true;
        }
        else
        {
            setState(0);
            result = false;
        }
    }
    return result;
}

bool ImageView::onCommand(UINT code, HWND ctrl, UINT id)
{
    if ((code | 1) != 1 || ctrl)
        return Doc::onCommand(code, ctrl, id);
    if (id == 0xE123)
    {
        clipboard_setBitmap(m_bitmap.m_bitmap);
        return true;
    }
    if (id != 0xE104)
        return Doc::onCommand(code, ctrl, id);
    HBITMAP bitmap = m_bitmap.m_bitmap;
    if (bitmap)
    {
        String path;
        int kind = m_field58;
        COMDLG_FILTERSPEC* filters;
        if (kind == kKindBmp)
            filters = g_bmpFileFilter;
        else if (kind == kKindDds)
            filters = g_ddsFileFilters;
        else
        {
            filters = g_tgaFileFilters;
            if (kind != kKindTga)
                filters = 0;
        }
        DWORD typeIndex = 1;
        if (!FileDialogs_save(m_hWnd, &path, 0, filters, (kind != kKindBmp) + 1, &typeIndex, 0))
        {
            path.removeExtension();
            const wchar_t* ext;
            if (m_field58 == kKindBmp || typeIndex == 2)
                ext = L".bmp";
            else if (m_field58 == kKindDds)
                ext = L".dds";
            else
                ext = L".tga";
            path << ext;
            if (path.endsWithNoCase(L"bmp"))
            {
                Bytes out(0, 0u);
                if (!writeBmp(out, bitmap))
                    static_cast<aStream&>(static_cast<iStream&>(out)).save(path.m_buffer);
            }
            else
            {
                static_cast<aStream&>(static_cast<iStream&>(m_bytes)).save(path.m_buffer);
            }
        }
    }
    return true;
}

bool ImageView::onKey(WPARAM vk, LPARAM flags, bool down)
{
    if (down && (short)vk == 'C' && GetKeyState(VK_CONTROL) < 0)
    {
        clipboard_setBitmap(m_bitmap.m_bitmap);
        return true;
    }
    if (GetKeyState(VK_CONTROL) < 0 && (short)vk == 'W')
    {
        onCommand(0, 0, 0xE102);
        return true;
    }
    return false;
}

void ImageView::onResized()
{
    m_bitmap.invalidate(true);
}

COMDLG_FILTERSPEC* ImageView::getFilters(int* count)
{
    *count = 1;
    return g_bmpFileFilter;
}
