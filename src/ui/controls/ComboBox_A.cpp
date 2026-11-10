#include "ComboBox.h"
#include "_imports_p06.h"

LRESULT ComboBox::getCurSel()
{
    return SendMessageW(m_hWnd, CB_GETCURSEL, 0, 0);
}
