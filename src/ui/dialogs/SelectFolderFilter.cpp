#include "SelectFolderFilter.h"
#include "String.hpp"
#include "FileDialogs.h"
#include "_imports_p10.h"

STDMETHODIMP SelectFolderFilter::QueryInterface(REFIID riid, void** ppv)
{
    if (memcmp(&riid, &IID_IUnknown, sizeof(IID)) == 0 || memcmp(&riid, &IID_IFileDialogEvents, sizeof(IID)) == 0)
    {
        *ppv = this;
        AddRef();
        return S_OK;
    }
    return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) SelectFolderFilter::AddRef()
{
    return InterlockedIncrement(&m_refCount);
}

STDMETHODIMP_(ULONG) SelectFolderFilter::Release()
{
    if (!InterlockedDecrement(&m_refCount))
        operator delete(this);
    return m_refCount;
}

STDMETHODIMP SelectFolderFilter::OnFileOk(IFileDialog* dialog)
{
    IShellItem* item = NULL;
    dialog->GetResult(&item);
    String path;
    bool got = FileDialogs_getItemPath(item, &path);
    BOOL result = !got || !m_validator(path.m_buffer);
    return result;
}
