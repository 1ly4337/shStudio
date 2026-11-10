#include "GradeEditor.h"
#include "_imports_A.h"

static void createLabel(HWND parent, const wchar_t* text, int x, int y, int cx, int cy)
{
    HWND label = CreateWindowExW(0, L"Static", text, WS_CHILD | WS_VISIBLE, x, y, cx, cy, parent, 0, 0, 0);
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(4), 0);
}

GradeEditor::~GradeEditor()
{
}

bool GradeEditor::create(HWND parent)
{
    m_buttons = 1;
    RECT rc;
    SetRect(&rc, CW_USEDEFAULT, CW_USEDEFAULT, 240, 150);
    if (!Window::create(parent, 0, &rc, 0x80C80000, 0x10101, L"Set Grade Value"))
        return false;
    m_owner = Modal_getTopLevelWindow(parent);
    Window_centerOn(m_hWnd, m_owner);
    show(true);
    return true;
}

bool GradeEditor::onCreate()
{
    createLabel(m_hWnd, L"Use this dialog to change the Grade of the selected items", 20, 15, 180, 24);
    createLabel(m_hWnd, L"New Grade value", 20, 50, 120, 18);
    m_gradeEdit.EditText::createNumber(m_hWnd, (HMENU)(INT_PTR)'grde', 150, 47, 40, 18, m_grade, 0, 0);
    return Modal::onCreate();
}

int GradeEditor::mapCommandToResult(int id)
{
    if (id == 1)
        m_grade = m_gradeEdit.EditText::getInt(0);
    return id;
}
