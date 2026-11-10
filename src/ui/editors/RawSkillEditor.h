#pragma once
#include <windows.h>
#include "ToonSkillEditor.h"
#include "Scroller.h"

class ToonSkill;

class RawSkillEditor : public ToonSkillEditor
{
public:
    explicit RawSkillEditor(ToonSkill* skill) : ToonSkillEditor(skill) {}
    virtual ~RawSkillEditor();

    virtual bool onCreate();
    virtual bool createEditor(HWND parent);

public:
    Scroller m_scroller;
    bool     m_compact;
};
static_assert(sizeof(RawSkillEditor) == 0x2B40, "RawSkillEditor size");
