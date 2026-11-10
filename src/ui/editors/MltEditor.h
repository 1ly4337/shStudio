#pragma once
#include <windows.h>
#include "Modal.h"
#include "String.hpp"
#include "EditText.h"
#include "Button.h"
#include "ComboBox.h"
#include "FlatButton.h"

class ModelMLT;

class MltEditor : public Modal
{
public:
    MltEditor(ModelMLT* model, const wchar_t* basePath);
    ~MltEditor() override;

    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    int mapCommandToResult(int id) override;

    bool create(HWND parent);

public:
    ModelMLT*  m_model;
    String     m_basePath;
    bool       m_readOnly;
    EditText   m_texture;
    EditText   m_vertex;
    FlatButton m_browseTexture;
    FlatButton m_browseVertex;
    ComboBox   m_alpha;
    Button     m_rename;
};
static_assert(sizeof(MltEditor) == 0x168, "MltEditor size");
