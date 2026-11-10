#pragma once
#include <windows.h>
#include <shobjidl.h>

typedef bool (__stdcall* FolderValidator)(const wchar_t* path);

class SelectFolderFilter : public IFileDialogEvents
{
public:
    SelectFolderFilter(FolderValidator validator) : m_validator(validator), m_refCount(1) {}

    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override;
    STDMETHODIMP_(ULONG) AddRef() override;
    STDMETHODIMP_(ULONG) Release() override;
    STDMETHODIMP OnFileOk(IFileDialog* dialog) override;
    STDMETHODIMP OnFolderChanging(IFileDialog* dialog, IShellItem* folder) override { return E_NOTIMPL; }
    STDMETHODIMP OnFolderChange(IFileDialog* dialog) override { return E_NOTIMPL; }
    STDMETHODIMP OnSelectionChange(IFileDialog* dialog) override { return E_NOTIMPL; }
    STDMETHODIMP OnShareViolation(IFileDialog* dialog, IShellItem* item, FDE_SHAREVIOLATION_RESPONSE* response) override { return E_NOTIMPL; }
    STDMETHODIMP OnTypeChange(IFileDialog* dialog) override { return E_NOTIMPL; }
    STDMETHODIMP OnOverwrite(IFileDialog* dialog, IShellItem* item, FDE_OVERWRITE_RESPONSE* response) override { return E_NOTIMPL; }

public:
    FolderValidator m_validator;
    volatile LONG   m_refCount;
};
static_assert(sizeof(SelectFolderFilter) == 0x0C, "SelectFolderFilter size");
