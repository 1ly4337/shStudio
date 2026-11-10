#include "DropSource.h"
#include "_imports_p10.h"

STDMETHODIMP DropSource::QueryInterface(REFIID riid, void** ppv)
{
    if (!ppv)
        return E_POINTER;
    *ppv = 0;
    if (IsEqualIID(riid, IID_IUnknown))
    {
        AddRef();
        *ppv = this;
        return S_OK;
    }
    if (IsEqualIID(riid, IID_IDropSource))
    {
        AddRef();
        *ppv = this;
        return S_OK;
    }
    return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) DropSource::AddRef()
{
    return InterlockedIncrement(&m_refCount);
}

STDMETHODIMP_(ULONG) DropSource::Release()
{
    LONG count = InterlockedDecrement(&m_refCount);
    if (!count)
    {
        delete this;
        return 0;
    }
    return count;
}

STDMETHODIMP DropSource::QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState)
{
    if (fEscapePressed)
        return DRAGDROP_S_CANCEL;
    if ((grfKeyState & (MK_LBUTTON | MK_RBUTTON)) == 0)
        return DRAGDROP_S_DROP;
    return S_OK;
}

STDMETHODIMP DropSource::GiveFeedback(DWORD dwEffect)
{
    return DRAGDROP_S_USEDEFAULTCURSORS;
}
