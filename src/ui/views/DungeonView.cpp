#include "p05_common.h"
#include "DungeonView.h"
#include "Dungeon.h"
#include "glViewer.h"
#include "FlatButton.h"
#include "UpDownCtrl.h"
#include "MouseMoveControler.h"
#include "StringTable.h"
#include "CtrlHelpers.h"

DungeonView::DungeonPanel::~DungeonPanel()
{
}

SIZE* DungeonView::getDefaultSize(SIZE* out, SIZE def)
{
    out->cx = 850;
    out->cy = def.cy;
    return out;
}

int DungeonView::getPanelWidth()
{
    return 330;
}

CtrlsPanel* DungeonView::createPanel()
{
    DungeonPanel* panel = new DungeonPanel(this, &m_viewer);
    if (!panel)
        return 0;
    return panel;
}

Drawable* DungeonView::createModel()
{
    return new Dungeon();
}

#define CP_ORIENT_BTN(p, i)  (((FlatButton*)((uint8_t*)(p) + 0x78)) + (i))
#define CP_ORIENT_EDIT(p, i) (((EditText*)((uint8_t*)(p) + 0xE4)) + (i))
#define CP_ORIENT_SPIN(p, i) (((UpDownCtrl*)((uint8_t*)(p) + 0x108)) + (i))
#define CP_POS_BTN(p, i)     (((FlatButton*)((uint8_t*)(p) + 0x12C)) + (i))
#define CP_POS_EDIT(p, i)    (((EditText*)((uint8_t*)(p) + 0x198)) + (i))

static void comboAdd(ComboBox& combo, const wchar_t* text)
{
    if (SendMessageW(combo.m_hWnd, CB_ADDSTRING, 0, (LPARAM)text) != CB_ERR) {
        LRESULT count = SendMessageW(combo.m_hWnd, CB_GETCOUNT, 0, 0);
        if (count >= 0) {
            StringTable::Entry* entry = new StringTable::Entry((long)count, text);
            combo.m_items.StringTable::add(entry);
        }
    }
}

static void createCombo(ComboBox& combo, HWND parent, int x, int y, int cx, int cy, UINT id)
{
    HWND h = CreateWindowExW(WS_EX_CLIENTEDGE, L"ComboBox", 0, (combo.m_dropLines & 3) | 0x50010000, x, y, cx, cy,
                             parent, (HMENU)(UINT_PTR)id, 0, 0);
    combo.m_hWnd = h;
    if (h)
        combo.setFontId(combo.m_fontId);
    else
        GetLastError();
}

static void createCheck(Button& button, HWND parent, const wchar_t* text, int x, int cx, UINT id)
{
    HWND h = CreateWindowExW(0, L"Button", text, 0x50010002, x, 94, cx, 18, parent, (HMENU)(UINT_PTR)id, 0, 0);
    button.m_hWnd = h;
    if (h)
        button.setFontId(button.m_fontId);
    else
        GetLastError();
}

bool DungeonView::DungeonPanel::onCreate()
{
    HANDLE strip = LoadImageW(hModule, (LPCWSTR)0xA9, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
    HWND label = CreateWindowExW(0, L"Static", L"Position:", WS_CHILD | WS_VISIBLE, 20, 12, 45, 14, m_hWnd, 0, 0, 0);
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(4), 0);
    int x = 65;
    for (int i = 0; i < 3; ++i) {
        UINT id = 230 + i;
        CP_POS_BTN(this, i)->create(m_hWnd, (HMENU)(UINT_PTR)id, x, 10, 18, 18, strip, id - 97);
        int editX = x + 20;
        EditText* edit = CP_POS_EDIT(this, i);
        HWND h = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", L"", 0x50010080, editX, 10, 55, 18, m_hWnd,
                                 (HMENU)(UINT_PTR)(id + 10), 0, 0);
        edit->m_hWnd = h;
        if (h) {
            edit->setFontId(edit->m_fontId);
            SetWindowSubclass(edit->m_hWnd, decimalEditSubclassProc, 0x65646974 , (DWORD_PTR)edit);
            wchar_t buffer[32];
            swprintf_s(buffer, 32, L"%.4f", 0.0);
            SendMessageW(edit->m_hWnd, WM_SETTEXT, 0, (LPARAM)buffer);
        }
        x = editX + 67;
    }
    label = CreateWindowExW(0, L"Static", L"Orient°:", WS_CHILD | WS_VISIBLE, 20, 36, 45, 14, m_hWnd, 0, 0, 0);
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(4), 0);
    x = 65;
    for (int i = 0; i < 3; ++i) {
        UINT id = 200 + i;
        CP_ORIENT_BTN(this, i)->create(m_hWnd, (HMENU)(UINT_PTR)id, x, 34, 18, 18, strip, id - 71);
        int editX = x + 20;
        EditText* edit = CP_ORIENT_EDIT(this, i);
        edit->createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 10), editX, 34, 55, 18, 0, 0, 0);
        HWND buddy = edit->m_hWnd;
        int spinX = editX + 55;
        UpDownCtrl* spin = CP_ORIENT_SPIN(this, i);
        HWND h = CreateWindowExW(0, L"msctls_updown32", 0, 0x50000067, spinX, 34, 18, 18, m_hWnd,
                                 (HMENU)(UINT_PTR)(id + 20), 0, 0);
        spin->m_hWnd = h;
        if (h) {
            spin->setFontId(spin->m_fontId);
            SendMessageW(spin->m_hWnd, UDM_SETBUDDY, (WPARAM)buddy, 0);
        } else {
            GetLastError();
        }
        SendMessageW(spin->m_hWnd, UDM_SETRANGE32, 0, 360);
        x = spinX + 12;
    }
    DeleteObject(strip);

    label = CreateWindowExW(0, L"Static", L"Mode:", WS_CHILD | WS_VISIBLE, 20, 68, 40, 16, m_hWnd, 0, 0, 0);
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(4), 0);
    createCombo(m_combo1, m_hWnd, 65, 64, 75, 60, 0x12C);
    comboAdd(m_combo1, L"Wireframe");
    comboAdd(m_combo1, L"Texture");
    SendMessageW(m_combo1.m_hWnd, CB_SETCURSEL, 1, 0);

    label = CreateWindowExW(0, L"Static", L"Camera:", WS_CHILD | WS_VISIBLE, 160, 68, 50, 16, m_hWnd, 0, 0, 0);
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(4), 0);
    createCombo(m_combo2, m_hWnd, 210, 64, 100, 60, 0x12D);
    comboAdd(m_combo2, L"External view");
    comboAdd(m_combo2, L"Thrid Person");
    SendMessageW(m_combo2.m_hWnd, CB_SETCURSEL, 0, 0);

    label = CreateWindowExW(0, L"Static", L"Options:", WS_CHILD | WS_VISIBLE, 20, 96, 50, 16, m_hWnd, 0, 0, 0);
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(4), 0);
    createCheck(m_btn2, m_hWnd, L" Use Alpha Blending", 65, 110, 0x12F);
    createCheck(m_btn1, m_hWnd, L" Draw bounding boxes", 185, 130, 0x12E);

    SIZE size;
    getSize(&size);
    RECT rc;
    SetRect(&rc, 20, 124, size.cx - 40, size.cy - 139);
    m_list.Window::create(m_hWnd, (HMENU)0x190, &rc, 0x50800000, 0, 0);

    glViewer* viewer = m_viewer;
    if (!viewer->m_mouse)
        viewer->m_mouse = new MouseMoveControler(&viewer->m_rotation, &viewer->m_position);
    viewer->m_mouseDown = false;
    return true;
}

void DungeonView::DungeonPanel::onSize(WPARAM , int cx, int cy)
{
    RECT rc;
    m_list.getRectInParent(&rc);
    int height = cy - rc.top - 15;
    if (height < 50)
        height = 50;
    m_list.resize(cx - 40, height);
}

bool DungeonView::loadModel()
{
    if (!ModuleView::loadModel())
        return false;
    Dungeon* dungeon = dynamic_cast<Dungeon*>(m_model);
    DungeonPanel* panel = dynamic_cast<DungeonPanel*>(m_panel);
    panel->m_list.setDungeon(dungeon);
    return true;
}
