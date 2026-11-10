#pragma once
#include <windows.h>
#include <ole2.h>
#include <shlobj.h>
#include <commctrl.h>

extern UINT g_cfShellIDListArray;
extern UINT g_cfFileNameW;
extern UINT g_cfFileContents;
extern UINT g_cfFileGroupDescriptorW;
extern UINT g_cfWorldFile;
extern bool g_clipboardFormatsRegistered;

bool registerClipboardFormats();
BOOL createDragImage(HIMAGELIST himl, int index, SHDRAGIMAGE* image);
HRESULT getFileDataObject(HWND hwndOwner, LPCWSTR path, void** ppv);
HRESULT initializeDragImage(SHDRAGIMAGE* image, IDataObject** dataObject);
bool isPerformedDropMove(IDataObject* dataObject);

struct DragImage
{
    DragImage();
    ~DragImage();

    HWND    m_hWnd;
    HDC     m_hdcMem;
    HDC     m_hdcScreen;
    HBITMAP m_hbmBackground;
    HBITMAP m_hbmImage;
    HGDIOBJ m_hbmOld;
    POINT   m_pos;
    POINT   m_hotSpot;
    SIZE    m_size;
    int     m_param30;
    int     m_param34;
    int     m_param38;
    HGLOBAL m_hData;
    bool    m_visible;
};
static_assert(sizeof(DragImage) == 0x44, "DragImage size");

extern DragImage* g_dragImage;

HGDIOBJ beginDragImage(int param30, int param34, int param38, HGLOBAL hData, HWND hWnd, HBITMAP image);
HWND showDragImage(const POINT* pos, const POINT* hotSpot);
int moveDragImage(const POINT* clientPt);
void endDragImage();
