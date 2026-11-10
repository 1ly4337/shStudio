#include "p05_common.h"
#include "EditText.h"

EditText::EditText()
{
}

void EditText::setFloat(float value)
{
    double v = value;
    const wchar_t* fmt;
    if (1.0 > v)
        fmt = L"%.4f";
    else if (100.0 > v)
        fmt = L"%.3f";
    else
        fmt = (1000.0 > v) ? L"%.2f" : L"%.1f";
    wchar_t buffer[32];
    swprintf_s(buffer, 32, fmt, v);
    SendMessageW(m_hWnd, WM_SETTEXT, 0, (LPARAM)buffer);
}
