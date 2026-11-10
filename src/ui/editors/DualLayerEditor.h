#pragma once
#include <windows.h>
#include "Modal.h"
#include "EditText.h"

class DualLayerModel;

class DualLayerEditor : public Modal
{
public:
    DualLayerEditor(LPCWSTR title, DualLayerModel* model);
    ~DualLayerEditor() override;

    bool onCreate() override;
    int mapCommandToResult(int id) override;

    bool create(HWND parent);

public:
    LPCWSTR         m_title;
    DualLayerModel* m_model;
    bool            m_readOnly;
    EditText        m_index;
    EditText        m_top;
    EditText        m_hands;
    EditText        m_bottom;
    EditText        m_shoes;
    EditText        m_empty;
    EditText        m_helmet;
};
static_assert(sizeof(DualLayerEditor) == 0x10C, "DualLayerEditor size");
