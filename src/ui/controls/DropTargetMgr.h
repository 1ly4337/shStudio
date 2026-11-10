#pragma once
#include <windows.h>
#include <ole2.h>
#include <shlobj.h>
#include "ptrArray.h"

class DropTargetMgr : public IDropTarget
{
public:
    explicit DropTargetMgr(HWND hWnd);

    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override;
    STDMETHODIMP_(ULONG) AddRef() override;
    STDMETHODIMP_(ULONG) Release() override;
    STDMETHODIMP DragEnter(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) override;
    STDMETHODIMP DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) override;
    STDMETHODIMP DragLeave() override;
    STDMETHODIMP Drop(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) override;
    virtual ~DropTargetMgr();
    virtual bool onDragOver(DWORD keyState, POINT* clientPt, DWORD* effect);
    virtual bool onDrop(FORMATETC* format, STGMEDIUM* medium, DWORD* effect) = 0;
    virtual void onDragLeave() {}

    HWND                 m_hWnd;
    ULONG                m_refCount;
    int                  m_unused0C;
    POINT                m_point;
    IDropTargetHelper*   m_helper;
    ptrArray<FORMATETC*> m_formats;
    FORMATETC*           m_format;
    STGMEDIUM            m_medium;
};
static_assert(sizeof(DropTargetMgr) == 0x44, "DropTargetMgr size");
