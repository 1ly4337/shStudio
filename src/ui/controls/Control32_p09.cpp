#include <windows.h>
#include <new>
#include "Control32.h"
#include "String.hpp"
#include "_imports_p09.h"
#include "_imports_g6.h"

bool Control32::setFont(HFONT font)
{
    if (!m_hWnd || !font)
        return false;
    SendMessageW(m_hWnd, WM_SETFONT, (WPARAM)font, 0);
    return true;
}

bool Control32::setFontId(int id)
{
    bool ok = setFont(getStdFont((unsigned)id));
    if (ok)
        m_fontId = id;
    return ok;
}

String* Control32::getText(String* out)
{
    new (out) String();
    if (m_hWnd)
    {
        LRESULT length = SendMessageW(m_hWnd, WM_GETTEXTLENGTH, 0, 0);
        if (length > 0)
        {
            WPARAM size = (WPARAM)(length + 1);
            out->reserve((unsigned)(length + 1));
            unsigned copied = (unsigned)SendMessageW(m_hWnd, WM_GETTEXT, size, (LPARAM)out->m_buffer);
            if (copied < out->m_capacity)
            {
                out->m_length = copied;
                out->m_buffer[copied] = 0;
            }
        }
    }
    return out;
}
