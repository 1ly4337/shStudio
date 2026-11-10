#include "dragdrop.h"
#include <cstring>
#include "_imports_p10.h"

UINT g_cfShellIDListArray;
UINT g_cfFileNameW;
UINT g_cfFileContents;
UINT g_cfFileGroupDescriptorW;
UINT g_cfWorldFile;
bool g_clipboardFormatsRegistered;
DragImage* g_dragImage;

bool registerClipboardFormats()
{
    bool result = g_clipboardFormatsRegistered;
    if (!g_clipboardFormatsRegistered)
    {
        g_cfShellIDListArray = 0;
        g_cfFileNameW = 0;
        g_cfFileContents = 0;
        g_cfFileGroupDescriptorW = 0;
        g_cfWorldFile = 0;
        g_cfShellIDListArray = RegisterClipboardFormatW(L"Shell IDList Array");
        g_cfFileNameW = RegisterClipboardFormatW(L"FileNameW");
        g_cfFileContents = RegisterClipboardFormatW(L"FileContents");
        g_cfFileGroupDescriptorW = RegisterClipboardFormatW(L"FileGroupDescriptorW");
        g_cfWorldFile = RegisterClipboardFormatW(L"WorldFile");
        result = true;
        g_clipboardFormatsRegistered = true;
    }
    return result;
}

BOOL createDragImage(HIMAGELIST himl, int index, SHDRAGIMAGE* image)
{
    memset(image, 0, sizeof(SHDRAGIMAGE));
    if (himl)
    {
        int cx, cy;
        ImageList_GetIconSize(himl, &cx, &cy);
        image->sizeDragImage.cx = cx;
        image->sizeDragImage.cy = cy;
        image->ptOffset.x = cx;
        image->ptOffset.y = cy;
        image->crColorKey = CLR_NONE;
        HDC hdc = CreateCompatibleDC(0);
        if (hdc)
        {
            HBITMAP bitmap = CreateBitmap(cx, cy, 1, 32, 0);
            image->hbmpDragImage = bitmap;
            if (bitmap)
            {
                HGDIOBJ old = SelectObject(hdc, bitmap);
                ImageList_Draw(himl, index, hdc, 0, 0, ILD_TRANSPARENT);
                SelectObject(hdc, old);
            }
            DeleteDC(hdc);
        }
    }
    return image->hbmpDragImage != 0;
}

HRESULT getFileDataObject(HWND hwndOwner, LPCWSTR path, void** ppv)
{
    SFGAOF attributes;
    LPCITEMIDLIST childPidl;
    LPITEMIDLIST pidl;
    *ppv = 0;
    HRESULT hr = SHParseDisplayName(path, 0, &pidl, 0, &attributes);
    if (hr < 0)
        return hr;
    IShellFolder* folder = 0;
    HRESULT result = SHBindToParent(pidl, IID_IShellFolder, (void**)&folder, &childPidl);
    if (result >= 0)
    {
        result = folder->GetUIObjectOf(hwndOwner, 1, &childPidl, IID_IDataObject, 0, ppv);
        folder->Release();
    }
    CoTaskMemFree(pidl);
    return result;
}

HRESULT initializeDragImage(SHDRAGIMAGE* image, IDataObject** dataObject)
{
    IDragSourceHelper* helper = 0;
    HRESULT hr = CoCreateInstance(CLSID_DragDropHelper, 0, CLSCTX_ALL, IID_IDragSourceHelper, (void**)&helper);
    if (hr >= 0)
    {
        hr = helper->InitializeFromBitmap(image, *dataObject);
        helper->Release();
    }
    DeleteObject(image->hbmpDragImage);
    return hr;
}

bool isPerformedDropMove(IDataObject* dataObject)
{
    bool moved = false;
    FORMATETC format;
    format.cfFormat = (CLIPFORMAT)RegisterClipboardFormatW(L"Performed DropEffect");
    format.ptd = 0;
    format.dwAspect = DVASPECT_CONTENT;
    format.lindex = -1;
    format.tymed = TYMED_HGLOBAL;
    STGMEDIUM medium;
    memset(&medium, 0, sizeof(medium));
    if (dataObject && dataObject->GetData(&format, &medium) >= 0)
    {
        if ((medium.tymed & TYMED_HGLOBAL) != 0 && GlobalSize(medium.hGlobal) >= 4)
        {
            moved = *(DWORD*)GlobalLock(medium.hGlobal) == DROPEFFECT_MOVE;
            GlobalUnlock(medium.hGlobal);
        }
        ReleaseStgMedium(&medium);
    }
    return moved;
}

DragImage::DragImage()
{
    m_pos.x = 0;
    m_pos.y = 0;
    m_hotSpot.x = m_pos.x;
    m_param34 = 0;
    m_param38 = 0;
    m_hData = 0;
    m_hWnd = 0;
    m_size.cy = 0;
    m_size.cx = 0;
    m_hotSpot.y = m_pos.y;
    m_visible = false;
    m_hdcScreen = CreateDCW(L"DISPLAY", 0, 0, 0);
    m_hdcMem = CreateCompatibleDC(m_hdcScreen);
    m_hbmImage = 0;
    m_hbmBackground = 0;
    m_hbmOld = 0;
}

DragImage::~DragImage()
{
    if (m_hData)
        GlobalFree(m_hData);
    if (m_hdcMem)
    {
        if (m_hbmBackground)
        {
            SelectObject(m_hdcMem, m_hbmOld);
            DeleteObject(m_hbmBackground);
        }
        DeleteDC(m_hdcMem);
    }
    if (m_hdcScreen)
        DeleteDC(m_hdcScreen);
    if (m_hbmImage)
        DeleteObject(m_hbmImage);
}

HGDIOBJ beginDragImage(int param30, int param34, int param38, HGLOBAL hData, HWND hWnd, HBITMAP image)
{
    if (g_dragImage)
    {
        endDragImage();
        DragImage* previous = g_dragImage;
        if (previous)
            delete previous;
    }
    DragImage* drag = new DragImage();
    drag->m_param38 = param38;
    drag->m_hData = hData;
    drag->m_hWnd = hWnd;
    int cx = 0;
    int cy = 0;
    g_dragImage = drag;
    drag->m_param30 = param30;
    drag->m_param34 = param34;
    drag->m_hbmImage = image;
    if (image)
    {
        BITMAP bm;
        GetObjectW(image, sizeof(BITMAP), &bm);
        cx = bm.bmWidth;
        cy = bm.bmHeight;
        drag = g_dragImage;
    }
    drag->m_size.cx = cx;
    drag->m_size.cy = cy;
    HBITMAP background = CreateCompatibleBitmap(drag->m_hdcScreen, cx, cy);
    g_dragImage->m_hbmBackground = background;
    HGDIOBJ old = SelectObject(g_dragImage->m_hdcMem, background);
    g_dragImage->m_hbmOld = old;
    return old;
}

HWND showDragImage(const POINT* pos, const POINT* hotSpot)
{
    DragImage* drag = g_dragImage;
    drag->m_pos.x = pos->x;
    drag->m_pos.y = pos->y;
    drag->m_hotSpot.x = hotSpot->x;
    drag->m_hotSpot.y = hotSpot->y;
    if (!drag->m_visible)
    {
        BitBlt(drag->m_hdcMem, 0, 0, drag->m_size.cx, drag->m_size.cy,
               drag->m_hdcScreen, drag->m_pos.x, drag->m_pos.y, SRCCOPY);
        drawBitmapTransparent(g_dragImage->m_hdcScreen, &g_dragImage->m_pos, g_dragImage->m_hbmImage);
        drag = g_dragImage;
        g_dragImage->m_visible = true;
    }
    return SetCapture(drag->m_hWnd);
}

int moveDragImage(const POINT* clientPt)
{
    DragImage* drag = g_dragImage;
    if (g_dragImage && g_dragImage->m_visible)
    {
        BitBlt(g_dragImage->m_hdcScreen, g_dragImage->m_pos.x, g_dragImage->m_pos.y,
               g_dragImage->m_size.cx, g_dragImage->m_size.cy, g_dragImage->m_hdcMem, 0, 0, SRCCOPY);
        drag = g_dragImage;
        g_dragImage->m_visible = false;
    }
    POINT point = *clientPt;
    ClientToScreen(drag->m_hWnd, &point);
    drag = g_dragImage;
    g_dragImage->m_pos.x = point.x - g_dragImage->m_hotSpot.x;
    int result = point.y - drag->m_hotSpot.y;
    drag->m_pos.y = result;
    if (!drag->m_visible)
    {
        BitBlt(drag->m_hdcMem, 0, 0, drag->m_size.cx, drag->m_size.cy,
               drag->m_hdcScreen, drag->m_pos.x, result, SRCCOPY);
        drawBitmapTransparent(g_dragImage->m_hdcScreen, &g_dragImage->m_pos, g_dragImage->m_hbmImage);
        result = (int)(INT_PTR)g_dragImage;
        g_dragImage->m_visible = true;
    }
    return result;
}

void endDragImage()
{
    if (g_dragImage && g_dragImage->m_visible)
    {
        BitBlt(g_dragImage->m_hdcScreen, g_dragImage->m_pos.x, g_dragImage->m_pos.y,
               g_dragImage->m_size.cx, g_dragImage->m_size.cy, g_dragImage->m_hdcMem, 0, 0, SRCCOPY);
        g_dragImage->m_visible = false;
    }
    ReleaseCapture();
    DragImage* drag = g_dragImage;
    if (g_dragImage)
        delete drag;
    g_dragImage = 0;
}
