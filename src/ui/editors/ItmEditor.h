#pragma once
#include <windows.h>
#include "Modal.h"
#include "String.hpp"
#include "EditText.h"
#include "Button.h"
#include "ComboBox.h"
#include "FlatButton.h"
#include "ColorChooser.h"

class ItemModel;

class ItmEditor : public Modal
{
public:
    ItmEditor(ItemModel* model, const wchar_t* basePath);
    ~ItmEditor() override;

    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    int mapCommandToResult(int id) override;

    bool create(HWND parent);

public:
    ItemModel*   m_model;
    String       m_basePath;
    bool         m_readOnly;
    EditText     m_texture;
    EditText     m_vertex;
    FlatButton   m_browseTexture;
    FlatButton   m_browseVertex;
    ComboBox     m_glow;
    EditText     m_unk2;
    EditText     m_unk3;
    Button       m_extended;
    ColorChooser m_color;
    EditText     m_rotation;
    EditText     m_scale;
    EditText     m_unk5;
    Button       m_rename;
};
static_assert(sizeof(ItmEditor) == 0x1C0, "ItmEditor size");
