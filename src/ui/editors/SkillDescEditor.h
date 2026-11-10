#pragma once
#include <windows.h>
#include "Modal.h"
#include "EditText.h"
#include "String.hpp"

class SkillDescEditor : public Modal
{
public:
    SkillDescEditor(String* texts, int count);
    virtual ~SkillDescEditor();

    DWORD getStyle() override;
    bool onCreate() override;
    bool onGetMinMaxInfo(MINMAXINFO* mmi) override;
    void onSize(WPARAM type, int cx, int cy) override;
    int mapCommandToResult(int id) override;

    virtual bool createEditor(HWND parent);
    virtual int createControls();
    virtual int loadValues();
    virtual int saveValues();

public:
    int      m_height;
    int      m_count;
    String*  m_texts;
    EditText m_edit;
    EditText m_edits[15];
};
static_assert(sizeof(SkillDescEditor) == 0x178, "SkillDescEditor size");
