#include "NpcView.h"
#include "NpcData.h"
#include "QuestsList.h"
#include "QuestDetails.h"
#include "Serializable.h"
#include "Bytes.h"
#include "Preferences.h"
#include "ida_compat.h"
#include "_imports_E.h"
#include <shlwapi.h>

NpcView::NpcView(void* file, Serializable* data)
    : SDataView(file, data),
      m_tab(this)
{
    NpcData* npcData;
    if (data)
    {
        npcData = dynamic_cast<NpcData*>(data);
    }
    else
    {
        npcData = new NpcData();
        m_content = static_cast<Serializable*>(npcData);
        m_object = static_cast<Serializable*>(npcData);
    }
    m_npcData = npcData;
    m_tab.m_data = npcData;
}

NpcView::~NpcView()
{
}

bool NpcView::onCreate()
{
    if (!DocView::onCreate())
        return false;
    HWND parent = m_hWnd;
    RECT buffer;
    RECT* rc = getClientRect(&buffer);
    int top = rc->top;
    if (!m_tab.create(parent, (HMENU)0x65, rc->left, top, rc->right - rc->left, rc->bottom - top,
                      0x50000000, 0, rc->bottom - top))
        return false;
    m_tab.fillLists();
    return true;
}

void NpcView::onActivate(bool active)
{
    m_tab.onActivate(active);
}

bool NpcView::onCommand(UINT code, HWND ctrl, UINT id)
{
    if ((code | 1) == 1 && !ctrl)
    {
        if (id == 0xE103)
        {
            if (save())
            {
                String dir = g_preferences.getString(L"Server Data", L"location", 0);
                if (dir.m_length)
                {
                    BOOL exists = PathFileExistsW((dir + L"Bin\\Data\\").m_buffer);
                    if (exists)
                        dir << L"Bin\\Data\\";
                    Bytes bytes;
                    m_npcData->write(bytes, 'file');
                    bytes.aStream::save((dir + L"NpcQuest.SData").m_buffer);
                }
            }
            return true;
        }
        if (id == 0xE460)
        {
            QuestsList* list = dynamic_cast<QuestsList*>(m_tab.getActivePage());
            if (!list)
                return true;
            QuestDetails* quest = dynamic_cast<QuestDetails*>(list->getNode(list->getNextSelected(0)));
            if (!quest)
                return true;
            String path;
            const wchar_t* filters[2] = { L"Text File", L"*.txt" };
            int filterIndex = 0;
            if (!FileDialogs_save(m_hWnd, &path, L".txt", (const COMDLG_FILTERSPEC*)filters, 1, (DWORD*)&filterIndex, L"Save sentences as"))
                quest->m_bytes.save(path.m_buffer);
            return true;
        }
    }
    if (SDataView::onCommand(code, ctrl, id))
        return true;
    return m_tab.onCommand(code, (LPARAM)ctrl, (WORD)id) != 0;
}

bool NpcView::onInitMenu(HMENU menu)
{
    SDataView::onInitMenu(menu);
    m_tab.onInitMenu((WPARAM)menu);
    return true;
}

void NpcView::updateMenu(HMENU menu)
{
    DocView::updateMenu(menu);
    MENUITEMINFOW mii;
    memset(&mii.fMask, 0, 0x2C);
    mii.cbSize = sizeof(MENUITEMINFOW);
    mii.fMask = MIIM_STATE;
    mii.fState = MFS_DISABLED;
    SetMenuItemInfoW(menu, 0xE105, FALSE, &mii);
    memset(&mii.fMask, 0, 0x2C);
    mii.cbSize = sizeof(MENUITEMINFOW);
    mii.fMask = MIIM_STATE;
    mii.fState = MFS_DISABLED;
    SetMenuItemInfoW(menu, 0xE12B, FALSE, &mii);
    memset(&mii.fMask, 0, 0x2C);
    mii.cbSize = sizeof(MENUITEMINFOW);
    mii.fMask = MIIM_STATE | MIIM_STRING;
    mii.fState = 0;
    mii.dwTypeData = (LPWSTR)L"Episo 5 format (3 rewards per quest)";
    mii.cch = 36;
    SetMenuItemInfoW(menu, 0xE402, FALSE, &mii);
    memset(&mii.fMask, 0, 0x2C);
    mii.cbSize = sizeof(MENUITEMINFOW);
    mii.fMask = MIIM_STATE | MIIM_STRING;
    mii.fState = 0;
    mii.dwTypeData = (LPWSTR)L"Episo 6 format (6 rewards per quest)";
    mii.cch = 36;
    SetMenuItemInfoW(menu, 0xE403, FALSE, &mii);
}

void NpcView::fillScriptMenu(HMENU menu)
{
    appendMenuItem(menu, 0xE435);
    appendMenuItem(menu, 0xE430);
    appendMenuItem(menu, 0xE431);
    MENUITEMINFOW mi;
    memset(&mi, 0, sizeof(mi));
    mi.cbSize = sizeof(MENUITEMINFOW);
    mi.fMask = MIIM_STATE | MIIM_ID | MIIM_STRING;
    mi.fType = 0;
    mi.wID = 0xE460;
    mi.fState = 0;
    mi.dwTypeData = (LPWSTR)L"Export sentences of a quest...";
    InsertMenuItemW(menu, 0xFFFFFFFF, FALSE, &mi);
}

HRESULT NpcView::load()
{
    HRESULT result = DocView::load();
    if (!result)
        m_tab.fillLists();
    return result;
}

bool NpcView::exportData(String* out, DWORD format)
{
    return m_npcData->toText(*out, format);
}

void NpcView::setViewMode(int mode)
{
    if (mode)
        m_npcData->setCodePage(mode);
}

int NpcView::getViewMode()
{
    return m_npcData->m_format & 0xFFF;
}
