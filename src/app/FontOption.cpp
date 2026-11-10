#include "FontOption.h"
#include "IniFile.h"
#include "RegKey.h"
#include "Strings.h"
#include "gdi_cache.h"
#include "g2_inline.h"
#include "_imports_p07.h"
#include <cstring>
#include <cwchar>

FontOption::FontOption()
{
    m_updating = false;
    m_unused144 = 0;
    m_font = 0;
    m_sampleText = L"ABCDEFGH abcdefgh 0123456789\r\n¾Æº§\r\n"
                   L"Turkish: ÀÐàð ÍÝíý ÎÞîþ";
}

FontOption::~FontOption()
{
}

int CALLBACK FontOption::enumFontProc(const LOGFONTW* lf, const TEXTMETRICW* tm, DWORD fontType, LPARAM lParam)
{
    if (lf->lfCharSet != SYMBOL_CHARSET)
    {
        if (lf->lfWeight == FW_NORMAL || wcscmp(lf->lfFaceName, L"Tahoma") == 0)
        {
            const WCHAR* face = lf->lfFaceName;
            if (face && *face != L'@' && (face[0] != 0x5241 || (BYTE)face[1] != 0x20))
                ((Strings*)lParam)->add(face);
        }
    }
    return 1;
}

void FontOption::fillFontNames(ComboBox& combo, const String& selected)
{
    Strings names;
    HDC dc = GetDC(0);
    LOGFONTW lf;
    memset(&lf, 0, sizeof(lf));
    lf.lfCharSet = DEFAULT_CHARSET;
    EnumFontFamiliesExW(dc, &lf, (FONTENUMPROCW)enumFontProc, (LPARAM)&names, 0);
    ReleaseDC(0, dc);

    Strings::compare_string compare;
    if (names.m_count >= 2)
    {
        names.m_changes = 0;
        names.sort(0, names.m_count - 1, true );
    }

    WPARAM selection = 0;
    int count = names.m_count;
    for (int i = 0; i < count; )
    {
        const wchar_t* face = 0;
        if (i >= 0 && i < names.m_count && names.m_items[i])
            face = names.m_items[i]->m_buffer;
        String name(face);
        if (name.m_length == selected.m_length)
        {
            if (_wcsnicmp(name.m_buffer, selected.m_buffer, name.m_length) == 0)
                selection = i;
        }
        ++i;
        combo.addItem(name.m_buffer, i);
    }
    SendMessageW(combo.m_hWnd, CB_SETCURSEL, selection, 0);
}

void FontOption::load(IniFile* prefs)
{
    m_fontName = prefs->getString(L"Layout", L"font name", L"Tahoma");
    m_fontSize = prefs->getInt(L"Layout", L"font size", 13);
    m_codePage = prefs->getInt(L"Layout", L"code page", 0);
    m_sizeCombo.selectData(m_fontSize);
    m_codePageCombo.selectData(m_codePage);
    m_codePageEdit.setInt(m_codePage);
    fillFontNames(m_fontCombo, m_fontName);
    SetWindowPos(m_fontCombo.m_hWnd, 0, 0, 0, 200, 270, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
    m_sample.setFont(createCodePageFont(m_codePage, m_fontSize, m_fontName.m_buffer));
    m_sample.setText(m_sampleText.m_buffer);
}

void FontOption::save(IniFile* prefs)
{
    {
        alignas(String) unsigned char storage[sizeof(String)];
        String* text = m_fontCombo.getText(reinterpret_cast<String*>(storage));
        m_fontName = *text;
        text->~String();
    }
    m_fontSize = (int)m_sizeCombo.getSelectedData();
    m_codePage = (int)m_codePageCombo.getSelectedData();
    if (m_codePage == -1)
        m_codePage = m_codePageEdit.getInt(-1);
    prefs->writeString(L"Layout", L"font name", m_fontName.m_buffer);
    prefs->writeInt(L"Layout", L"font size", m_fontSize);
    prefs->writeInt(L"Layout", L"code page", m_codePage);
    HFONT layoutFont = g_stdFonts[6];
    if (m_font)
        layoutFont = m_font;
    g_stdFonts[6] = layoutFont;
    g_layoutCodePage = m_codePage;
}

bool FontOption::onCreate()
{
    g2_createStatic(m_hWnd, L"Font used to display localized strings", 15, 10, 200, 16, 4);
    g2_createComboEx(m_fontCombo, m_hWnd, 15, 27, 200, 270, 0x65, 1);
    g2_createStatic(m_hWnd, L"Font Size", 225, 10, 75, 16, 4);
    g2_createComboEx(m_sizeCombo, m_hWnd, 225, 27, 75, 270, 0x66, 1);
    m_sizeCombo.addItem(L"10", 10);
    m_sizeCombo.addItem(L"11", 11);
    m_sizeCombo.addItem(L"12", 12);
    m_sizeCombo.addItem(L"13", 13);
    m_sizeCombo.addItem(L"14", 14);
    m_sizeCombo.addItem(L"16", 16);
    m_sizeCombo.addItem(L"18", 18);
    m_sizeCombo.addItem(L"20", 20);
    m_sizeCombo.addItem(L"24", 24);
    SetWindowPos(m_sizeCombo.m_hWnd, 0, 0, 0, 75, 270, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);

    g2_createStatic(m_hWnd, L"ACP:", 15, 63, 30, 16, 4);
    g2_createComboEx(m_codePageCombo, m_hWnd, 45, 57, 170, 190, 0x67, 0x101);
    m_codePageCombo.addItem(L"...", -1);
    m_codePageCombo.addItem(L"ANSI", 0);
    m_codePageCombo.addItem(L"OEM Character Set", 1);
    m_codePageCombo.addItem(L"Arabic", 1256);
    m_codePageCombo.addItem(L"Baltic (ISO 8859-13*)", 1257);
    m_codePageCombo.addItem(L"Central Europe (ISO 8859-2*)", 1250);
    m_codePageCombo.addItem(L"Chinese Mainland", 936);
    m_codePageCombo.addItem(L"Chinese Taiwan (Big5)", 950);
    m_codePageCombo.addItem(L"Cyrillic", 1251);
    m_codePageCombo.addItem(L"Greek (ISO 8859-7*)", 1253);
    m_codePageCombo.addItem(L"Hebrew (ISO 8859-8)", 1255);
    m_codePageCombo.addItem(L"Japanese", 932);
    m_codePageCombo.addItem(L"Korean, Hangul", 949);
    m_codePageCombo.addItem(L"Korean, Johab", 1361);
    m_codePageCombo.addItem(L"Latin 1 (ISO 8859-1)", 1252);
    m_codePageCombo.addItem(L"Latin 2 (ISO 8859-2*)", 1250);
    m_codePageCombo.addItem(L"Latin 3 (ISO 8859-3)", 28593);
    m_codePageCombo.addItem(L"Latin 4 (ISO 8859-4)", 28594);
    m_codePageCombo.addItem(L"Latin 5 (ISO 8859-9)", 1254);
    m_codePageCombo.addItem(L"Thai  (ISO 8859-11*)", 874);
    m_codePageCombo.addItem(L"Turkish (ISO 8859-9)", 1254);
    m_codePageCombo.addItem(L"Vietnamese", 1258);
    m_codePageEdit.createNumber(m_hWnd,(HMENU) (HMENU)104, 225, 59, 75, 20, 0, 0x1000, 0);

    RECT rect;
    memset(&rect, 0, sizeof(rect));
    GetWindowRect(m_codePageCombo.m_hWnd, &rect);
    int y = rect.bottom - rect.top + 69;
    SetWindowPos(m_codePageCombo.m_hWnd, 0, 0, 0, 170, 190, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
    int height = 90;
    {
        RegKey key(HKEY_LOCAL_MACHINE, L"System\\CurrentControlSet\\Control\\NLS\\CodePage", KEY_READ);
        if (key.m_open)
        {
            String acp;
            if (key.getString(L"ACP", &acp))
            {
                int codePage = acp.toInt();
                if (codePage != (int)0x80000000)
                {
                    String text(L"System ACP (ANSI code page): ");
                    text << (long)codePage;
                    WCHAR localeName[128];
                    memset(localeName, 0, sizeof(localeName));
                    GetUserDefaultLocaleName(localeName, 128);
                    text << " (" << localeName << ')';
                    g2_createStatic(m_hWnd, text.m_buffer, 15, y, 285, 16, 4);
                    y += 24;
                    height = 66;
                }
            }
        }
        g2_createEdit(m_sample, m_hWnd, 0x50211044, 15, y, 285, height, 0x6E);
    }
    return true;
}

bool FontOption::onCommand(UINT code, HWND ctrl, UINT id)
{
    if (code == CBN_SELCHANGE)
    {
        if (ctrl == m_fontCombo.m_hWnd || ctrl == m_sizeCombo.m_hWnd || ctrl == m_codePageCombo.m_hWnd)
        {
            if (!m_updating)
            {
                {
                    alignas(String) unsigned char storage[sizeof(String)];
                    String* text = m_fontCombo.getText(reinterpret_cast<String*>(storage));
                    m_fontName = *text;
                    text->~String();
                }
                m_fontSize = (int)m_sizeCombo.getSelectedData();
                m_codePage = (int)m_codePageCombo.getSelectedData();
                m_updating = true;
                m_codePageEdit.setInt(m_codePage);
                m_updating = false;
                if (m_font)
                    DeleteObject(m_font);
                m_font = createCodePageFont(m_codePage, m_fontSize, m_fontName.m_buffer);
                m_sample.setFont(m_font);
                m_sample.setText(m_sampleText.m_buffer);
            }
            return true;
        }
        return Container::onCommand(code, ctrl, id);
    }
    if (code != EN_CHANGE || ctrl != m_codePageEdit.m_hWnd)
        return Container::onCommand(code, ctrl, id);
    if (m_updating)
        return true;

    UINT codePage = m_codePageEdit.getInt(m_codePage);
    m_codePage = codePage;
    String name;
    CPINFOEXW info;
    memset(&info, 0, sizeof(info));
    if (GetCPInfoExW(codePage, 0, &info))
        name << info.CodePageName;
    else
        (name << (long)m_codePage) << L" (Invalid)";
    m_codePageCombo.setItemText(0 , name.m_buffer);
    m_updating = true;
    m_codePageCombo.selectData((WPARAM)-1);
    m_updating = false;
    return true;
}
