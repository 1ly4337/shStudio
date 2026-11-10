#include "ChooseCodepageDialog.h"
#include "Window.h"
#include "CodepageDialog_impl.h"
#include "_imports_p10.h"

int g_lastChosenCodepage = -1;

int ChooseCodepageDialog::ListCtrlCP::findCodepage(int codepage)
{
    return CodepageDialogImpl::findCodepage(this, codepage);
}

bool ChooseCodepageDialog::ListCtrlCP::onCreate()
{
    return CodepageDialogImpl::listOnCreate(this);
}

void ChooseCodepageDialog::ListCtrlCP::onItemChanged(int item, UINT state)
{
    CodepageDialogImpl::listOnItemChanged(this, item, state);
}

ChooseCodepageDialog::ChooseCodepageDialog(const wchar_t* fileName, int unused)
    : m_list(this)
{
    int codepage = g_lastChosenCodepage;
    m_fileName.append(fileName, (unsigned)-1);
    if (codepage < 0)
        codepage = 0;
    m_cpEdit.m_hWnd = NULL;
    m_cpEdit.m_fontId = 0;
    m_codepage = codepage;
}

bool ChooseCodepageDialog::create(HWND parent)
{
    m_buttons = MB_OKCANCEL;
    RECT rc;
    SetRect(&rc, CW_USEDEFAULT, CW_USEDEFAULT, 380, 340);
    if (!Window::create(parent, NULL, &rc, 0x80C80000, 0x10101, L"Codepage Selection"))
        return false;
    m_owner = Modal_getTopLevelWindow(parent);
    Window_centerOn(m_hWnd, m_owner);
    show(true);
    return true;
}

bool ChooseCodepageDialog::onCreate()
{
    String text;
    text.append(L"Please select the codepage to be used to decode the sentences of\n", (unsigned)-1);
    unsigned len = m_fileName.m_length;
    if (len && m_fileName.m_buffer[len - 1] == L'\\')
        text << "the workspace ";
    else if (len >= 4 && !_wcsnicmp(m_fileName.m_buffer + len - 4, L".saf", 4))
        text << "the archive ";
    else
        text << "the file ";
    text.append(m_fileName.m_buffer, m_fileName.m_length);

    HWND st = CreateWindowExW(0, L"Static", text.m_buffer, 0x50000000, 10, 10, 320, 40, m_hWnd, NULL, NULL, NULL);
    if (st)
        SendMessageW(st, WM_SETFONT, (WPARAM)getStdFont(4), 0);
    m_list.Control::create(m_hWnd, (HMENU)0x66, 10, 50, 340, 140, 0, 0, 0);
    ListCtrl::insertRow(&m_list, L"932", L"Japanese (Shift_JIS)", (const wchar_t*)0);
    ListCtrl::insertRow(&m_list, L"936", L"Chinese (Simplified) (GB2312)", (const wchar_t*)0);
    ListCtrl::insertRow(&m_list, L"949", L"Korean (EUC-KR)", (const wchar_t*)0);
    ListCtrl::insertRow(&m_list, L"1250", L"Central European", (const wchar_t*)0);
    ListCtrl::insertRow(&m_list, L"1251", L"Cyrillic", (const wchar_t*)0);
    ListCtrl::insertRow(&m_list, L"1252", L"Western European (Latin 1)", (const wchar_t*)0);
    return CodepageDialogImpl::createCodepageControls(this, 194);
}

void ChooseCodepageDialog::onShowDialog()
{
    CodepageDialogImpl::onShowDialog(this);
}

bool ChooseCodepageDialog::onCommand(UINT code, HWND ctrl, UINT id)
{
    return CodepageDialogImpl::onCommand(this, code, ctrl, id);
}

ChooseCodepageDialog::~ChooseCodepageDialog()
{
}

int ChooseCodepageDialog::run(HWND parent, const wchar_t* fileName)
{
    ChooseCodepageDialog dlg(fileName, (int)(INT_PTR)parent);
    int result;
    if (dlg.create(parent) && dlg.doModal() == IDOK)
    {
        result = dlg.m_codepage;
        g_lastChosenCodepage = result;
    }
    else
    {
        result = -1;
    }
    return result;
}
