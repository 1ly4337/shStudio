#pragma once
#include <windows.h>
#include "Modal.h"
#include "EditText.h"

class AlertDial : public Modal
{
public:
    AlertDial();
    virtual ~AlertDial();

    virtual LRESULT defWindowProc(UINT msg, WPARAM wParam, LPARAM lParam);
    virtual int mapCommandToResult(int id);

    virtual bool createAlert(HWND parent, int flags, LPCWSTR text, LPCWSTR details, LPCWSTR title);
    virtual bool createSimpleAlert(HWND parent, int flags, LPCWSTR text, LPCWSTR title);

    static int alert(HWND parent, LPCWSTR text, LPCWSTR title, int flags);
    static int productListError(HWND parent, LPCWSTR details);
    static bool confirm(HWND parent, LPCWSTR text);

public:
    LPCWSTR  m_text;
    LPCWSTR  m_details;
    RECT     m_textRect;
    HICON    m_icon;
    HBITMAP  m_gradient;
    EditText m_detailsEdit;
};
static_assert(sizeof(AlertDial) == 0xD8, "AlertDial size");
