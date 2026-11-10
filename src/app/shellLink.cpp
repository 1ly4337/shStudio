#include <windows.h>
#include <objbase.h>
#include <shlobj.h>
#include "shellLink.h"

HRESULT resolveShortcut(LPCWSTR link, LPWSTR target)
{
    HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (hr != S_OK && hr != S_FALSE)
        return hr;
    IShellLinkW* shellLink = 0;
    hr = CoCreateInstance(CLSID_ShellLink, 0, CLSCTX_INPROC_SERVER, IID_IShellLinkW, (void**)&shellLink);
    if (hr == S_OK)
    {
        IPersistFile* file = 0;
        hr = shellLink->QueryInterface(IID_IPersistFile, (void**)&file);
        if (hr == S_OK)
        {
            hr = file->Load(link, 0);
            if (hr == S_OK)
            {
                hr = shellLink->Resolve(0, 0x49);
                if (hr == S_OK)
                    hr = shellLink->GetPath(target, 512, 0, SLGP_UNCPRIORITY);
            }
            file->Release();
        }
        shellLink->Release();
    }
    CoUninitialize();
    return hr;
}
