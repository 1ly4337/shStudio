#include "TransformWeaponEditor.h"
#include "TransformWeaponModel.h"
#include "StudioFrame.h"
#include "g2_inline.h"
#include "_imports_p07.h"

TransformWeaponEditor::TransformWeaponEditor(TransformWeaponModel* model)
    : m_model(model)
{
}

TransformWeaponEditor::~TransformWeaponEditor()
{
}

bool TransformWeaponEditor::create(HWND parent)
{
    m_readOnly = g_studioFrame->isArchiveReadOnly();
    m_buttons = MB_OKCANCEL;
    RECT rc;
    SetRect(&rc, (int)0x80000000, (int)0x80000000, 240, 200);
    bool result = Window::create(parent, 0, &rc, 0x80C80000, 0x10101, L"Transform Weapon Model");
    if (result)
    {
        m_owner = Modal_getTopLevelWindow(parent);
        Window_centerOn(m_hWnd, m_owner);
        show(true);
        return true;
    }
    return result;
}

bool TransformWeaponEditor::onCreate()
{
    g2_createStatic(m_hWnd, L"Type:", 20, 22, 80, 16, 4);
    m_type.createNumber(m_hWnd,(HMENU) (HMENU)(UINT_PTR)'type', 100, 20, 50, 18, m_model->m_type, 0, 0);
    SendMessageW(m_type.m_hWnd, EM_SETLIMITTEXT, 3, 0);
    g2_createStatic(m_hWnd, L"Weapon 1:", 20, 46, 80, 16, 4);
    m_weapon1.createNumber(m_hWnd,(HMENU) (HMENU)(UINT_PTR)'itm1', 100, 44, 50, 18, m_model->m_weapon, 0, 0);
    SendMessageW(m_weapon1.m_hWnd, EM_SETLIMITTEXT, 3, 0);
    g2_createStatic(m_hWnd, L"Weapon 2:", 20, 70, 80, 16, 4);
    m_weapon2.createNumber(m_hWnd,(HMENU) (HMENU)(UINT_PTR)'itm2', 100, 68, 50, 18, m_model->m_weapon1, 0, 0);
    SendMessageW(m_weapon2.m_hWnd, EM_SETLIMITTEXT, 3, 0);
    return Modal::onCreate();
}

int TransformWeaponEditor::mapCommandToResult(int id)
{
    if (id == IDOK)
    {
        m_model->m_type = m_type.getInt(-1);
        m_model->m_weapon = m_weapon1.getInt(-1);
        m_model->m_weapon1 = m_weapon2.getInt(-1);
    }
    return id;
}
