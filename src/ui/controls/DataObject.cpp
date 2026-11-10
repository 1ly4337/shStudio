#include "DataObject.h"
#include <shlobj.h>
#include <cstring>
#include "_imports_p10.h"

DataObject::DataObject(const FORMATETC* formats, const STGMEDIUM* media, int count)
{
    m_refCount = 1;
    m_count = count;
    m_formats = new FORMATETC[count];
    m_media = new STGMEDIUM[count];
    for (int i = 0; i < count; ++i)
    {
        m_formats[i] = formats[i];
        m_media[i] = media[i];
    }
}

DataObject::~DataObject()
{
    if (m_formats)
        delete[] m_formats;
    if (m_media)
    {
        for (int i = 0; i < m_count; ++i)
            ReleaseStgMedium(&m_media[i]);
        delete[] m_media;
    }
}

STDMETHODIMP DataObject::QueryInterface(REFIID riid, void** ppv)
{
    if (!ppv)
        return E_POINTER;
    *ppv = 0;
    if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDataObject))
    {
        *ppv = this;
        AddRef();
        return S_OK;
    }
    return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) DataObject::AddRef()
{
    return InterlockedIncrement(&m_refCount);
}

STDMETHODIMP_(ULONG) DataObject::Release()
{
    LONG count = InterlockedDecrement(&m_refCount);
    if (!count)
    {
        delete this;
        return 0;
    }
    return count;
}

STDMETHODIMP DataObject::GetData(FORMATETC* pformatetcIn, STGMEDIUM* pmedium)
{
    if (!pformatetcIn || !pmedium)
        return E_POINTER;
    pmedium->tymed = 0;
    pmedium->hGlobal = 0;
    pmedium->pUnkForRelease = 0;
    int index = findFormat(pformatetcIn);
    if (index == -1)
        return DV_E_FORMATETC;
    FORMATETC* format = &m_formats[index];
    pmedium->tymed = format->tymed;
    return copyMedium(format, &m_media[index], pmedium);
}

STDMETHODIMP DataObject::GetDataHere(FORMATETC* pformatetc, STGMEDIUM* pmedium)
{
    if (!pformatetc || !pmedium)
        return E_POINTER;
    pmedium->tymed = 0;
    pmedium->hGlobal = 0;
    pmedium->pUnkForRelease = 0;
    int index = findFormat(pformatetc);
    if (index == -1)
        return DV_E_FORMATETC;
    FORMATETC* format = &m_formats[index];
    if ((format->tymed & (TYMED_ISTREAM | TYMED_ISTORAGE)) == 0)
        return DV_E_FORMATETC;
    return copyMediumHere(format, &m_media[index], pmedium);
}

STDMETHODIMP DataObject::QueryGetData(FORMATETC* pformatetc)
{
    if (!pformatetc)
        return E_POINTER;
    if (findFormat(pformatetc) == -1)
        return DV_E_FORMATETC;
    return S_OK;
}

STDMETHODIMP DataObject::GetCanonicalFormatEtc(FORMATETC* pformatectIn, FORMATETC* pformatetcOut)
{
    if (!pformatectIn || !pformatetcOut)
        return E_POINTER;
    memset(pformatetcOut, 0, sizeof(FORMATETC));
    return E_NOTIMPL;
}

STDMETHODIMP DataObject::SetData(FORMATETC* pformatetc, STGMEDIUM* pmedium, BOOL fRelease)
{
    if (!pformatetc || !pmedium)
        return E_POINTER;
    if (!fRelease)
        return E_NOTIMPL;
    ReleaseStgMedium(pmedium);
    pmedium->tymed = 0;
    pmedium->hGlobal = 0;
    pmedium->pUnkForRelease = 0;
    return S_OK;
}

STDMETHODIMP DataObject::EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC** ppenumFormatEtc)
{
    if (!ppenumFormatEtc)
        return E_POINTER;
    if (dwDirection == DATADIR_GET)
        return SHCreateStdEnumFmtEtc(m_count, m_formats, ppenumFormatEtc);
    return E_NOTIMPL;
}

STDMETHODIMP DataObject::DAdvise(FORMATETC*, DWORD, IAdviseSink*, DWORD*)
{
    return OLE_E_ADVISENOTSUPPORTED;
}

STDMETHODIMP DataObject::DUnadvise(DWORD)
{
    return OLE_E_ADVISENOTSUPPORTED;
}

STDMETHODIMP DataObject::EnumDAdvise(IEnumSTATDATA**)
{
    return OLE_E_ADVISENOTSUPPORTED;
}

HRESULT DataObject::copyMedium(const FORMATETC* format, const STGMEDIUM* source, STGMEDIUM* target)
{
    if (format->tymed != TYMED_HGLOBAL)
        return DV_E_FORMATETC;
    HGLOBAL hSource = source->hGlobal;
    SIZE_T size = GlobalSize(hSource);
    const void* src = GlobalLock(hSource);
    HGLOBAL hCopy = GlobalAlloc(GHND, size);
    void* dst = GlobalLock(hCopy);
    memcpy(dst, src, size);
    GlobalUnlock(hCopy);
    GlobalUnlock(hSource);
    target->hGlobal = hCopy;
    return S_OK;
}

HRESULT DataObject::copyMediumHere(const FORMATETC* format, const STGMEDIUM* source, STGMEDIUM* target)
{
    DWORD tymed = format->tymed;
    if (tymed == TYMED_ISTREAM || tymed == TYMED_ISTORAGE)
        target->pstm = 0;
    return DV_E_FORMATETC;
}

int DataObject::findFormat(const FORMATETC* format)
{
    int count = m_count;
    if (count <= 0)
        return -1;
    int index = 0;
    for (FORMATETC* f = m_formats;
         (format->tymed & f->tymed) == 0 || f->cfFormat != format->cfFormat || f->dwAspect != format->dwAspect;
         ++f)
    {
        if (++index >= count)
            return -1;
    }
    return index;
}
