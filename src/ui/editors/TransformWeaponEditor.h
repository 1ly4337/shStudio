#pragma once
#include "Modal.h"
#include "EditText.h"

class TransformWeaponModel;

class TransformWeaponEditor : public Modal
{
public:
    TransformWeaponEditor(TransformWeaponModel* model);
    virtual ~TransformWeaponEditor();
    bool create(HWND parent);
    bool onCreate() override;
    int mapCommandToResult(int id) override;

public:
    TransformWeaponModel* m_model;
    bool                  m_readOnly;
    EditText              m_type;
    EditText              m_weapon1;
    EditText              m_weapon2;
};
static_assert(sizeof(TransformWeaponEditor) == 0xD8, "TransformWeaponEditor size");
