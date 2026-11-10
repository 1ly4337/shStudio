#include "DropTargetMgr.h"
#include "_imports_p10.h"

DropTargetMgr::DropTargetMgr(HWND hWnd)
    : m_formats()
{
    m_point.y = 0;
    m_point.x = 0;
    m_refCount = 0;
    m_hWnd = hWnd;
    m_helper = 0;
    m_format = 0;
    m_medium.tymed = 0;
    m_medium.hGlobal = 0;
    m_medium.pUnkForRelease = 0;
    RegisterDragDrop(hWnd, this);
    CoCreateInstance(CLSID_DragDropHelper, 0, CLSCTX_INPROC_SERVER, IID_IDropTargetHelper, (void**)&m_helper);
}

DropTargetMgr::~DropTargetMgr()
{
    ReleaseStgMedium(&m_medium);
    if (m_helper)
        m_helper->Release();
    RevokeDragDrop(m_hWnd);
}

STDMETHODIMP DropTargetMgr::QueryInterface(REFIID riid, void** ppv)
{
    if (!ppv)
        return E_POINTER;
    *ppv = 0;
    if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDropTarget))
    {
        *ppv = this;
        AddRef();
        return S_OK;
    }
    return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) DropTargetMgr::AddRef()
{
    return ++m_refCount;
}

STDMETHODIMP_(ULONG) DropTargetMgr::Release()
{
    bool last = m_refCount-- == 1;
    ULONG count = m_refCount;
    if (last)
        delete this;
    return count;
}

STDMETHODIMP DropTargetMgr::DragEnter(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
    if (!pDataObj || !pdwEffect)
        return E_INVALIDARG;
    if (m_helper)
        m_helper->DragEnter(m_hWnd, pDataObj, (POINT*)&pt, *pdwEffect);
    m_format = 0;
    m_point.x = pt.x;
    m_point.y = pt.y;
    ScreenToClient(m_hWnd, &m_point);
    ReleaseStgMedium(&m_medium);
    int i = 0;
    if (m_formats.size() > 0)
    {
        while (pDataObj->QueryGetData(m_formats.m_items[i]))
        {
            if (++i >= m_formats.size())
                goto chosen;
        }
        m_format = m_formats.m_items[i];
    }
chosen:
    if (m_format)
    {
        if (m_helper)
            m_helper->Show(TRUE);
        pDataObj->GetData(m_format, &m_medium);
        if (!onDragOver(grfKeyState, &m_point, pdwEffect))
            *pdwEffect = 0;
        return S_OK;
    }
    *pdwEffect = 0;
    return S_OK;
}

STDMETHODIMP DropTargetMgr::DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
    if (!pdwEffect)
        return E_INVALIDARG;
    if (m_helper)
        m_helper->DragOver((POINT*)&pt, *pdwEffect);
    m_point.x = pt.x;
    m_point.y = pt.y;
    ScreenToClient(m_hWnd, &m_point);
    if (!onDragOver(grfKeyState, &m_point, pdwEffect))
        *pdwEffect = 0;
    return S_OK;
}

STDMETHODIMP DropTargetMgr::DragLeave()
{
    if (m_helper)
    {
        m_helper->Show(FALSE);
        m_helper->DragLeave();
    }
    onDragLeave();
    ReleaseStgMedium(&m_medium);
    m_format = 0;
    return S_OK;
}

STDMETHODIMP DropTargetMgr::Drop(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
    if (!pDataObj)
        return E_INVALIDARG;
    if (!pdwEffect)
        return E_INVALIDARG;
    if (m_helper)
        m_helper->Drop(pDataObj, (POINT*)&pt, *pdwEffect);
    m_point.x = pt.x;
    m_point.y = pt.y;
    ScreenToClient(m_hWnd, &m_point);
    onDragLeave();
    if (onDragOver(grfKeyState, &m_point, pdwEffect))
    {
        if (m_format)
            onDrop(m_format, &m_medium, pdwEffect);
    }
    m_format = 0;
    ReleaseStgMedium(&m_medium);
    return S_OK;
}

bool DropTargetMgr::onDragOver(DWORD keyState, POINT* clientPt, DWORD* effect)
{
    DWORD allowed = *effect;
    *effect = 0;
    if (!m_format)
        return false;
    BYTE keys = (BYTE)keyState;
    if (keys & MK_CONTROL)
    {
        DWORD wanted = DROPEFFECT_COPY;
        if (keys & MK_SHIFT)
            wanted = DROPEFFECT_LINK;
        *effect = wanted;
    }
    else if (keys & MK_SHIFT)
    {
        *effect = DROPEFFECT_MOVE;
    }
    else
    {
        if (allowed & DROPEFFECT_COPY)
        {
            *effect = DROPEFFECT_COPY;
            return *effect != 0;
        }
        if (allowed & DROPEFFECT_MOVE)
            *effect = DROPEFFECT_MOVE;
        else
            *effect = allowed & DROPEFFECT_LINK;
        return *effect != 0;
    }
    *effect = (allowed & *effect) != 0 ? *effect : 0;
    return *effect != 0;
}
