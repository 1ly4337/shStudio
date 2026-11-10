#pragma once
#include <windows.h>
#include "Modal.h"
#include "ListCtrl.h"
#include "String.hpp"
#include "Strings.h"
#include "EditText.h"
#include "Button.h"

class MonsterModel;

class MonEditor : public Modal
{
public:
    class MonList3D : public ListCtrl
    {
    public:
        MonList3D(const String& folder, MonsterModel* model);
        ~MonList3D() override;

        DWORD getStyle() override { return 5; }
        bool onCreate() override;
        Node* getNode(int index) override { return 0; }
        DWORD getExStyle() override { return 0x21; }
        HRESULT onGetDispInfo(NMLVDISPINFOW* di) override;
        bool removeItem(int index) override;
        virtual bool setItemString(int index, int column, const wchar_t* text);

    public:
        String  m_folder;
        Strings m_vertices;
        Strings m_textures;
    };

public:
    MonEditor(MonsterModel* model, const String& folder);
    ~MonEditor() override;

    bool create(HWND parent);

    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    int mapCommandToResult(int id) override;

public:
    MonsterModel* m_model;
    String        m_folder;
    EditText      m_name;
    MonList3D     m_list;
    Button        m_addFiles;
    EditText      m_animations[18];
    EditText      m_unknown;
    EditText      m_height;
};
static_assert(sizeof(MonEditor::MonList3D) == 0x16C, "MonEditor::MonList3D size");
static_assert(sizeof(MonEditor) == 0x344, "MonEditor size");
