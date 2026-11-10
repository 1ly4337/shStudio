#include "clipboard_A.h"
#include "Clipboard.h"
#include "_imports_p06.h"

bool clipboard_putBitmap(HBITMAP bitmap)
{
    if (!OpenClipboard(g_clipboard.m_hOwner))
        return false;
    HBITMAP copy = copyBitmap((const HGDIOBJ*)&bitmap);
    if (!copy)
    {
        CloseClipboard();
        return false;
    }
    SetClipboardData(CF_BITMAP, copy);
    CloseClipboard();
    return true;
}

bool clipboard_setBitmap(HBITMAP bitmap)
{
    if (!OpenClipboard(g_clipboard.m_hOwner))
        return false;
    HBITMAP copy = copyBitmap((const HGDIOBJ*)&bitmap);
    if (!copy)
    {
        CloseClipboard();
        return false;
    }
    EmptyClipboard();
    SetClipboardData(CF_BITMAP, copy);
    CloseClipboard();
    return true;
}
