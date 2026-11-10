#pragma once
#include "OptionPanel.h"
#include "String.hpp"
#include "ComboBox.h"
#include "EditText.h"

class FontOption : public OptionPanel
{
public:
    FontOption();
    virtual ~FontOption();

    bool onCreate() override;
    bool onCommand(UINT code, HWND ctrl, UINT id) override;
    void load(IniFile* prefs) override;
    void save(IniFile* prefs) override;

    static int CALLBACK enumFontProc(const LOGFONTW* lf, const TEXTMETRICW* tm, DWORD fontType, LPARAM lParam);
    static void fillFontNames(ComboBox& combo, const String& selected);

public:
    ComboBox m_fontCombo;
    ComboBox m_sizeCombo;
    ComboBox m_codePageCombo;
    EditText m_codePageEdit;
    EditText m_sample;
    String   m_fontName;
    String   m_sampleText;
    HFONT    m_font;
    int      m_fontSize;
    int      m_codePage;
    int      m_unused144;
    bool     m_updating;
};
static_assert(sizeof(FontOption) == 0x14C, "FontOption size");
