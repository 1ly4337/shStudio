#include "p05_common.h"
#include "Button.h"

Button::Button()
{
}

LRESULT Button::setCheck(bool checked)
{
    return SendMessageW(m_hWnd, BM_SETCHECK, checked, 0);
}

BOOL Button::isChecked()
{
    return SendMessageW(m_hWnd, BM_GETCHECK, 0, 0) == BST_CHECKED;
}
