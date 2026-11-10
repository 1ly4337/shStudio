#include "SDataFileOption.h"
#include "IniFile.h"
#include "String.hpp"
#include "g2_inline.h"
#include "_imports_p07.h"

SDataFileOption::SDataFileOption()
{
}

SDataFileOption::~SDataFileOption()
{
}

void SDataFileOption::load(IniFile* prefs)
{
    m_cleanCR = prefs->getInt(L"NpcQuest", L"clean CR", 1) == 1;
    m_cleanMerchant = prefs->getInt(L"NpcQuest", L"clean merchant", 1) == 1;
    m_cleanCRCheck.setCheck(m_cleanCR);
    m_languageCombo.selectData(prefs->getInt(L"SData", L"language", 840));
    {
        String primary = prefs->getString(L"SData", L"Primary", L"undefined");
        m_dbPrimary = primary.m_length == 7 && _wcsnicmp(primary.m_buffer, L"DBfiles", 7) == 0;
    }
    m_dbPrimaryCheck.setCheck(m_dbPrimary);
}

void SDataFileOption::save(IniFile* prefs)
{
    prefs->writeInt(L"NpcQuest", L"clean CR", m_cleanCR != 0);
    prefs->writeInt(L"NpcQuest", L"clean merchant", m_cleanMerchant != 0);
    prefs->writeInt(L"SData", L"language", (int)m_languageCombo.getSelectedData());
    const wchar_t* primary = L"DBfiles";
    if (!m_dbPrimary)
        primary = L"regular";
    prefs->writeString(L"SData", L"Primary", primary);
}

bool SDataFileOption::onCreate()
{
    g2_createStatic(m_hWnd, L"Options applied to DB .sdata files", 15, 10, 250, 16, 5);
    g2_createCheckBox(m_cleanCRCheck, m_hWnd, L" Remove line-feeds at end of strings", 15, 27, 250, 18, 0x65);
    g2_createCheckBox(m_cleanMerchantCheck, m_hWnd, L" Remove null items from list of merchant items",
                      15, 44, 250, 18, 0x67);
    g2_createCheckBox(m_dbPrimaryCheck, m_hWnd, L" DB files are used as primary source of data", 15, 70, 240, 18,
                      0x66);
    g2_createStatic(m_hWnd,
                    L"Cross-loading of sdata is often required, for instance to list mobs on a .svmap or items sold "
                    L"by merchants. DBxxxData files, vs former .sdata, are used depending on this setting.",
                    15, 88, 290, 40, 4);
    g2_createStatic(m_hWnd, L"Prefered Language:", 15, 140, 100, 16, 4);
    g2_createComboEx(m_languageCombo, m_hWnd, 115, 135, 150, 250, 0x67, 1);
    m_languageCombo.addItem(L"Chinese", 156);
    m_languageCombo.addItem(L"English", 840);
    m_languageCombo.addItem(L"Français", 250);
    m_languageCombo.addItem(L"Deutsch", 276);
    m_languageCombo.addItem(L"Italiano", 380);
    m_languageCombo.addItem(L"Korean", 410);
    m_languageCombo.addItem(L"Polski", 616);
    m_languageCombo.addItem(L"Türkçe", 792);
    g2_createStatic(m_hWnd,
                    L"Chosen language is looked for and used when a language independant version of a .sdata "
                    L"file is loaded.",
                    15, 162, 290, 32, 4);
    return true;
}

bool SDataFileOption::onCommand(UINT code, HWND ctrl, UINT id)
{
    if (code)
        return Container::onCommand(code, ctrl, id);
    if (id == 101)
    {
        m_cleanCR = !m_cleanCR;
        m_cleanCRCheck.setCheck(m_cleanCR);
        return true;
    }
    if (id == 102)
    {
        m_dbPrimary = !m_dbPrimary;
        m_dbPrimaryCheck.setCheck(m_dbPrimary);
        return true;
    }
    if (id != 103)
        return Container::onCommand(code, ctrl, id);
    m_cleanMerchant = !m_cleanMerchant;
    m_cleanMerchantCheck.setCheck(m_cleanMerchant);
    return true;
}
