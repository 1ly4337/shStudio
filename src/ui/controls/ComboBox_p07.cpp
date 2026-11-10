#include "ComboBox.h"
#include "_imports_p07.h"

LRESULT ComboBox::getSelectedData()
{
    HWND hWnd = m_hWnd;
    if (hWnd)
    {
        LRESULT selection = SendMessageW(hWnd, CB_GETCURSEL, 0, 0);
        return SendMessageW(m_hWnd, CB_GETITEMDATA, selection, 0);
    }
    return (LRESULT)hWnd;
}
