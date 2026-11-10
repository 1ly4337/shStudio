#pragma once
#include <windows.h>
#include <ole2.h>

class DropSource : public IDropSource
{
public:
    DropSource() : m_refCount(1) {}

    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override;
    STDMETHODIMP_(ULONG) AddRef() override;
    STDMETHODIMP_(ULONG) Release() override;
    STDMETHODIMP QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState) override;
    STDMETHODIMP GiveFeedback(DWORD dwEffect) override;
    virtual ~DropSource() {}

    LONG m_refCount;
};
static_assert(sizeof(DropSource) == 8, "DropSource size");
