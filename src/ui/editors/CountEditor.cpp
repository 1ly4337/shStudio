#include "CountEditor.h"
#include "_imports_A.h"

static void createLabel(HWND parent, const wchar_t* text, int x, int y, int cx, int cy)
{
    HWND label = CreateWindowExW(0, L"Static", text, WS_CHILD | WS_VISIBLE, x, y, cx, cy, parent, 0, 0, 0);
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(4), 0);
}

CountEditor::CountEditor()
{
    m_type = 0;
    m_typeId = 0;
    m_count = 1;
}

CountEditor::~CountEditor()
{
}

bool CountEditor::create(HWND parent)
{
    m_buttons = 1;
    RECT rc;
    SetRect(&rc, CW_USEDEFAULT, CW_USEDEFAULT, 240, 200);
    if (!Window::create(parent, 0, &rc, 0x80C80000, 0x10101, L"Bulk Count Edit"))
        return false;
    m_owner = Modal_getTopLevelWindow(parent);
    Window_centerOn(m_hWnd, m_owner);
    show(true);
    return true;
}

bool CountEditor::onCreate()
{
    createLabel(m_hWnd, L"Use this dialog to change the Count field of a range of items", 20, 15, 180, 30);
    createLabel(m_hWnd, L"Type of Items (required)", 20, 54, 120, 18);
    m_typeEdit.EditText::createNumber(m_hWnd, (HMENU)(INT_PTR)'type', 150, 51, 40, 18, m_type, 0, 0);
    createLabel(m_hWnd, L"TypeID of Items (0 for all)", 20, 78, 150, 18);
    m_typeIdEdit.EditText::createNumber(m_hWnd, (HMENU)(INT_PTR)'tpID', 150, 75, 40, 18, m_typeId, 0, 0);
    createLabel(m_hWnd, L"Count value", 20, 102, 150, 18);
    m_countEdit.EditText::createNumber(m_hWnd, (HMENU)(INT_PTR)'cnt ', 150, 99, 40, 18, m_count, 0, 0);
    return Modal::onCreate();
}

int CountEditor::mapCommandToResult(int id)
{
    if (id == 1)
    {
        m_type = m_typeEdit.EditText::getInt(0);
        m_typeId = m_typeIdEdit.EditText::getInt(0);
        m_count = m_countEdit.EditText::getInt(0);
    }
    return id;
}
