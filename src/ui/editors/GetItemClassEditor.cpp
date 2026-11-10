#include "GetItemClassEditor.h"
#include "ItemList.h"
#include "_imports_A.h"
#include <commctrl.h>

static void createLabel(HWND parent, const wchar_t* text, int x, int y, int cx, int cy)
{
    HWND label = CreateWindowExW(0, L"Static", text, WS_CHILD | WS_VISIBLE, x, y, cx, cy, parent, 0, 0, 0);
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(4), 0);
}

static void createCheckBox(Button& button, HWND parent, const wchar_t* text, DWORD style, int x, int y, int cx,
                           int id, bool subclass)
{
    button.m_hWnd = CreateWindowExW(0, L"Button", text, style, x, y, cx, 20, parent, (HMENU)(INT_PTR)id, 0, 0);
    if (button.m_hWnd)
        button.setFontId(button.m_fontId);
    else
        GetLastError();
    if (subclass && button.m_hWnd)
        SetWindowSubclass(button.m_hWnd, noEraseSubclassProc, 'chck', (DWORD_PTR)&button);
}

static void addUsedBy(ComboBox& combo, const wchar_t* text)
{
    if (SendMessageW(combo.m_hWnd, CB_ADDSTRING, 0, (LPARAM)text) != CB_ERR)
    {
        LRESULT count = SendMessageW(combo.m_hWnd, CB_GETCOUNT, 0, 0);
        combo.m_items.StringTable::put((int)count, text);
    }
}

GetItemClassEditor::GetItemClassEditor(ItemList* list, int faction)
    : m_list(list)
{
    m_faction = faction;
    memset(m_classChecked, 0, sizeof(m_classChecked));
    m_type = -1;
}

GetItemClassEditor::~GetItemClassEditor()
{
}

bool GetItemClassEditor::create(HWND parent)
{
    int extra = 20;
    if (m_faction >= 0)
        extra = 60;
    m_buttons = 1;
    RECT rc;
    SetRect(&rc, CW_USEDEFAULT, CW_USEDEFAULT, 330, extra + 180);
    if (!Window::create(parent, 0, &rc, 0x80C80000, 0x10101, L"Creating new item..."))
        return false;
    m_owner = Modal_getTopLevelWindow(parent);
    Window_centerOn(m_hWnd, m_owner);
    show(true);
    LRESULT count = SendMessageW(m_types.m_hWnd, CB_GETCOUNT, 0, 0);
    EnableWindow(m_btnOk, count > 0);
    return true;
}

bool GetItemClassEditor::onCreate()
{
    createLabel(m_hWnd,
                L"Use this dialog to define the type of the item to be created.\n"
                L"If the combo-box is empty, all available banks are already\n"
                L"full and no item can be created.",
                20, 15, 300, 38);
    createLabel(m_hWnd, L"Types:", 20, 67, 40, 18);

    m_types.m_hWnd = CreateWindowExW(0, L"ComboBoxEx32", 0, (m_types.m_dropLines & 3) | 0x50010000, 70, 63, 230, 200,
                                     m_hWnd, (HMENU)0x65, 0, 0);
    if (m_types.m_hWnd)
    {
        m_types.setFontId(m_types.m_fontId);
        SendMessageW(m_types.m_hWnd, CB_SETEXTENDEDUI, 1, 0);
        m_types.m_flags = 1;
    }
    else
    {
        GetLastError();
    }
    m_list->fillTypeCombo(&m_types);

    int faction = m_faction;
    HWND parent = m_hWnd;
    if (faction < 0)
    {
        createLabel(parent, L"Used by:", 20, 97, 50, 18);
        m_usedBy.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"ComboBox", 0, (m_usedBy.m_dropLines & 3) | 0x50010000,
                                          70, 93, 100, 120, m_hWnd, (HMENU)0x78, 0, 0);
        if (m_usedBy.m_hWnd)
            m_usedBy.setFontId(m_usedBy.m_fontId);
        else
            GetLastError();
        addUsedBy(m_usedBy, L"Human");
        addUsedBy(m_usedBy, L"Elf");
        addUsedBy(m_usedBy, L"All Lights");
        addUsedBy(m_usedBy, L"Deatheater");
        addUsedBy(m_usedBy, L"Vail");
        addUsedBy(m_usedBy, L"All Fury");
        addUsedBy(m_usedBy, L"All factions");
    }
    else
    {
        bool light = faction == 0;
        createLabel(parent, L"Will be equipped by:", 20, 95, 150, 18);
        createCheckBox(m_classes[0], m_hWnd, light ? L" Fighter" : L" Warrior", 0x50010002, 30, 113, 75, 0x6E, false);
        createCheckBox(m_classes[1], m_hWnd, light ? L" Defender" : L" Guardian", 0x50000002, 30, 133, 75, 0x6F, true);
        createCheckBox(m_classes[2], m_hWnd, light ? L" Ranger" : L" Assassin", 0x50000002, 120, 113, 60, 0x70, true);
        createCheckBox(m_classes[3], m_hWnd, light ? L" Archer" : L" Hunter", 0x50000002, 120, 133, 60, 0x71, true);
        createCheckBox(m_classes[4], m_hWnd, light ? L" Mage" : L" Pagan", 0x50000002, 210, 113, 60, 0x72, true);
        createCheckBox(m_classes[5], m_hWnd, light ? L" Priest" : L" Oracle", 0x50000002, 210, 133, 60, 0x73, true);
    }
    return Modal::onCreate();
}

bool GetItemClassEditor::onCommand(UINT code, HWND ctrl, UINT id)
{
    if (id - 110 > 5)
    {
        if (!code && id <= 11)
        {
            m_result = mapCommandToResult(id);
            if (m_result == 1 && m_modelessOwner)
                SendMessageW(m_modelessOwner->m_hWnd, 0x403, 1, 0);
            if (m_result != -1)
                PostMessageW(m_hWnd, WM_CLOSE, 0, 0);
        }
        return true;
    }
    Button& box = m_classes[id - 110];
    box.setCheck(box.isChecked() == 0);
    return true;
}

int GetItemClassEditor::mapCommandToResult(int id)
{
    if (id != 1)
        return id;
    LRESULT type;
    if (m_types.m_hWnd)
    {
        LRESULT selection = SendMessageW(m_types.m_hWnd, CB_GETCURSEL, 0, 0);
        type = SendMessageW(m_types.m_hWnd, CB_GETITEMDATA, selection, 0);
    }
    else
    {
        type = 0;
    }
    m_type = (int)type;
    if (type < 0)
        return -1;
    if (m_faction < 0)
    {
        LRESULT usedBy = SendMessageW(m_usedBy.m_hWnd, CB_GETCURSEL, 0, 0);
        m_usedByIndex = (int)usedBy;
        if (usedBy < 0)
            return -1;
        return id;
    }
    bool any = false;
    for (int i = 0; i < 6; ++i)
    {
        if (any || m_classes[i].isChecked())
            any = true;
        m_classChecked[i] = m_classes[i].isChecked() != 0;
    }
    if (!any)
        return -1;
    return 1;
}
