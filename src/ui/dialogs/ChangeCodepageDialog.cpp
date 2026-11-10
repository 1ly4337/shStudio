#include "ChangeCodepageDialog.h"
#include "Window.h"
#include "ida_compat.h"
#include "CodepageDialog_impl.h"
#include "_imports_p10.h"

int ChangeCodepageDialog::ListCtrlCP::findCodepage(int codepage)
{
    return CodepageDialogImpl::findCodepage(this, codepage);
}

bool ChangeCodepageDialog::ListCtrlCP::onCreate()
{
    return CodepageDialogImpl::listOnCreate(this);
}

void ChangeCodepageDialog::ListCtrlCP::onItemChanged(int item, UINT state)
{
    CodepageDialogImpl::listOnItemChanged(this, item, state);
}

ChangeCodepageDialog::ChangeCodepageDialog(const wchar_t* fileName, int codepage)
    : m_list(this)
{
    m_fileName.append(fileName, (unsigned)-1);
    if (codepage < 0)
        codepage = 0;
    m_cpEdit.m_hWnd = NULL;
    m_cpEdit.m_fontId = 0;
    m_codepage = codepage;
}

bool ChangeCodepageDialog::create(HWND parent)
{
    m_buttons = MB_OKCANCEL;
    RECT rc;
    SetRect(&rc, CW_USEDEFAULT, CW_USEDEFAULT, 380, 370);
    if (!Window::create(parent, NULL, &rc, 0x80C80000, 0x10101, L"Codepage Selection"))
        return false;
    m_owner = Modal_getTopLevelWindow(parent);
    Window_centerOn(m_hWnd, m_owner);
    show(true);
    return true;
}

bool ChangeCodepageDialog::onCreate()
{
    HWND st = CreateWindowExW(0, L"Static",
        L"This dialog will modify the codepage associated with the file.\n"
        L"Upon validation the file will be reloaded and decoded with the new selected codepage. When saved, the data"
        L" will be encoded with the new CP.",
        0x50000000, 10, 10, 340, 56, m_hWnd, NULL, NULL, NULL);
    if (st)
        SendMessageW(st, WM_SETFONT, (WPARAM)getStdFont(4), 0);
    m_list.Control::create(m_hWnd, (HMENU)0x66, 10, 66, 340, 160, 0, 0, 0);
    ListCtrl::insertRow(&m_list, L"932", L"Japanese (Shift_JIS)", (const wchar_t*)0);
    ListCtrl::insertRow(&m_list, L"936", L"Chinese (Simplified) (GB2312)", (const wchar_t*)0);
    ListCtrl::insertRow(&m_list, L"949", L"Korean (EUC-KR)", (const wchar_t*)0);
    ListCtrl::insertRow(&m_list, L"1250", L"Central European", (const wchar_t*)0);
    ListCtrl::insertRow(&m_list, L"1251", L"Cyrillic", (const wchar_t*)0);
    ListCtrl::insertRow(&m_list, L"1252", L"Western European (Latin 1)", (const wchar_t*)0);
    ListCtrl::insertRow(&m_list, L"65001", L"UTF8", (const wchar_t*)0);
    return CodepageDialogImpl::createCodepageControls(this, 230);
}

void ChangeCodepageDialog::onShowDialog()
{
    CodepageDialogImpl::onShowDialog(this);
}

bool ChangeCodepageDialog::onCommand(UINT code, HWND ctrl, UINT id)
{
    return CodepageDialogImpl::onCommand(this, code, ctrl, id);
}

ChangeCodepageDialog::~ChangeCodepageDialog()
{
}

int ChangeCodepageDialog::run(HWND parent, Document* doc)
{
    ChangeCodepageDialog dlg(FIELD(const wchar_t*, doc, 0x34), FIELD(int, doc, 0x140));
    int result;
    if (dlg.create(parent) && dlg.doModal() == IDOK)
        result = dlg.m_codepage;
    else
        result = -1;
    return result;
}
