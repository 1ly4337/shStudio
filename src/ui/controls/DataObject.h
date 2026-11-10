#pragma once
#include <windows.h>
#include <ole2.h>

class DataObject : public IDataObject
{
public:
    DataObject(const FORMATETC* formats, const STGMEDIUM* media, int count);
    ~DataObject();

    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override;
    STDMETHODIMP_(ULONG) AddRef() override;
    STDMETHODIMP_(ULONG) Release() override;
    STDMETHODIMP GetData(FORMATETC* pformatetcIn, STGMEDIUM* pmedium) override;
    STDMETHODIMP GetDataHere(FORMATETC* pformatetc, STGMEDIUM* pmedium) override;
    STDMETHODIMP QueryGetData(FORMATETC* pformatetc) override;
    STDMETHODIMP GetCanonicalFormatEtc(FORMATETC* pformatectIn, FORMATETC* pformatetcOut) override;
    STDMETHODIMP SetData(FORMATETC* pformatetc, STGMEDIUM* pmedium, BOOL fRelease) override;
    STDMETHODIMP EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC** ppenumFormatEtc) override;
    STDMETHODIMP DAdvise(FORMATETC* pformatetc, DWORD advf, IAdviseSink* pAdvSink, DWORD* pdwConnection) override;
    STDMETHODIMP DUnadvise(DWORD dwConnection) override;
    STDMETHODIMP EnumDAdvise(IEnumSTATDATA** ppenumAdvise) override;
    virtual HRESULT copyMedium(const FORMATETC* format, const STGMEDIUM* source, STGMEDIUM* target);
    virtual HRESULT copyMediumHere(const FORMATETC* format, const STGMEDIUM* source, STGMEDIUM* target);

    int findFormat(const FORMATETC* format);

    LONG       m_refCount;
    int        m_count;
    FORMATETC* m_formats;
    STGMEDIUM* m_media;
};
static_assert(sizeof(DataObject) == 0x14, "DataObject size");
