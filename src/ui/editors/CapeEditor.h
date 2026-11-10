#pragma once
#include "ItemEditor.h"
#include "ClothTextures.h"
#include "CapeModels.h"
#include "BmpCtrl.h"

class CapeEditor : public ItemEditor
{
public:
    CapeEditor(Window* owner, Data::Items* items, Data::Item* item);
    virtual ~CapeEditor();

    virtual bool onCreate();
    virtual bool onCommand(UINT code, HWND ctrl, UINT id);
    virtual int mapCommandToResult(int result);
    virtual bool createEditor();
    virtual void saveControls();

    bool showPreview(int index);
    const wchar_t* textureName(char faction, int index);

public:
    ClothTextures m_texDE;
    ClothTextures m_texEL;
    ClothTextures m_texHU;
    ClothTextures m_texVI;
    CapeModels    m_capeModels;
    EditText      m_modelEdit;
    Button        m_previewButton;
    BmpCtrl       m_preview;
    bool          m_previewShown;
};
static_assert(sizeof(CapeEditor) == 0x718, "CapeEditor size");
