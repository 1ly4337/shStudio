#include "Clipboard.h"
#include "iStream.h"
#include "oStream.h"
#include <cstring>
#include <cwchar>
#include "_imports_p10.h"

Clipboard g_clipboard;

bool Clipboard::setText(const wchar_t* text, bool empty)
{
    if (!text)
        return false;
    int length = (int)wcslen(text);
    if (length <= 0 || !OpenClipboard(g_clipboard.m_hOwner))
        return false;
    if (empty)
        EmptyClipboard();
    SIZE_T bytes = 2 * length + 2;
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (hMem)
    {
        void* p = GlobalLock(hMem);
        if (p)
            memcpy(p, text, bytes);
        GlobalUnlock(hMem);
        SetClipboardData(CF_UNICODETEXT, hMem);
    }
    CloseClipboard();
    return true;
}

bool Clipboard::setData(UINT format, oStream& data, bool empty)
{
    aStream& buffer = data;
    SIZE_T size = (SIZE_T)buffer.m_size;
    if (!size)
        return false;
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, size);
    void* p = GlobalLock(hMem);
    if (p)
        memcpy(p, buffer.m_data, size);
    GlobalUnlock(hMem);
    if (OpenClipboard(g_clipboard.m_hOwner))
    {
        if (empty)
            EmptyClipboard();
        SetClipboardData(format, hMem);
        CloseClipboard();
        g_clipboard.m_globals.add(hMem);
        return true;
    }
    GlobalFree(hMem);
    return false;
}

bool Clipboard::getData(UINT format, iStream& data)
{
    aStream& buffer = data;
    if (buffer.m_data && buffer.m_capacity)
        memset(buffer.m_data, 0, buffer.m_capacity);
    buffer.m_size = 0;
    buffer.m_pos = 0;
    if (!IsClipboardFormatAvailable(format) || !OpenClipboard(g_clipboard.m_hOwner))
        return false;
    bool ok = false;
    HANDLE hData = GetClipboardData(format);
    if (hData)
    {
        SIZE_T size = GlobalSize(hData);
        void* p = GlobalLock(hData);
        if (p)
        {
            ok = buffer.assign(p, (int)size);
            GlobalUnlock(hData);
        }
    }
    CloseClipboard();
    return ok;
}
