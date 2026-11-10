#pragma once
#include <shobjidl.h>
#include "SimpleView.h"
#include "EditText.h"
#include "String.hpp"

class FileTree;

class TextView : public SimpleView
{
public:
    TextView(FileTree* node, bool readOnly);
    ~TextView() override;
    void setText(String& text);

    LPCWSTR getClassName() override;
    void onRegisterClass(WNDCLASSEXW* wcx) override;
    void onActivate(bool active) override;
    void fillScriptMenu(HMENU scripts) override;
    int getMenuId() override;

    bool onCreate() override;
    bool onClose() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    HRESULT load() override;
    COMDLG_FILTERSPEC* getFilters(int* count) override;

    virtual bool saveText();
    virtual bool loadText();

public:
    EditText m_edit;
    String   m_text;
    int      m_84;
    bool     m_readOnly;
};
static_assert(sizeof(TextView) == 0x8C, "TextView size");
