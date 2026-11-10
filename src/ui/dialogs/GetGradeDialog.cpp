#include "GetGradeDialog.h"
#include "GradeSets.h"
#include "GradeSet.h"
#include "AlertDial.h"
#include "gdi_cache.h"
#include "_imports_p07.h"

GetGradeDialog::GetGradeDialog(GradeSets* sets)
    : Modal()
    , m_sets(sets)
    , m_grade(0)
    , m_edit()
{
}

GetGradeDialog::~GetGradeDialog()
{
}

bool GetGradeDialog::open(HWND parent)
{
    m_buttons = 1;
    RECT rc;
    SetRect(&rc, (int)0x80000000, (int)0x80000000, 240, 120);
    if (!create(parent, 0, &rc, 0x80C80000, 0x10101, L"Create Grade Bag"))
        return false;
    m_owner = Modal_getTopLevelWindow(parent);
    Window_centerOn(m_hWnd, m_owner);
    show(true);
    return true;
}

bool GetGradeDialog::onCreate()
{
    HWND label = CreateWindowExW(0, L"Static", L"ID of Grade Bag to create:", 0x50000000,
                                 20, 18, 130, 18, m_hWnd, 0, 0, 0);
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(4), 0);
    m_edit.createNumber(m_hWnd, (HMENU)'grde', 150, 15, 50, 18, (UINT)(int)m_grade, 0, 0);
    return Modal::onCreate();
}

void GetGradeDialog::onShowDialog()
{
    SendMessageW(m_edit.m_hWnd, EM_SETSEL, 0, -1);
    SetFocus(m_edit.m_hWnd);
}

int GetGradeDialog::mapCommandToResult(int id)
{
    if (id != 1)
        return id;
    m_grade = (short)m_edit.getInt(0);
    GradeSet* set = m_sets->find(m_grade);
    if (set)
        return 2 * (AlertDial::alert(m_hWnd, L"The bag already exists, do you want to use it?", 0,
                                     MB_ICONQUESTION | MB_YESNO) == IDYES) - 1;
    return id;
}
