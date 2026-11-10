#include "GateKeeperEditor.h"
#include <commctrl.h>
#include <cwchar>
#include <cstdlib>
#include "GateKeeper.h"
#include "_imports_D.h"

static inline void setFloatText(HWND hWnd, float value)
{
    wchar_t buffer[32];
    double v = value;
    if (v >= 1.0)
    {
        if (v >= 100.0)
        {
            const wchar_t* format = L"%.2f";
            if (v >= 1000.0)
                format = L"%.1f";
            swprintf_s(buffer, 0x20, format, v);
        }
        else
        {
            swprintf_s(buffer, 0x20, L"%.3f", v);
        }
    }
    else
    {
        swprintf_s(buffer, 0x20, L"%.4f", v);
    }
    SendMessageW(hWnd, WM_SETTEXT, 0, (LPARAM)buffer);
}

static inline void createDecimalEdit(EditText& edit, HWND parent, int x, int y, UINT id, float value)
{
    edit.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", L"", 0x50010080, x, y, 55, 18, parent, (HMENU)(UINT_PTR)id, 0, 0);
    if (edit.m_hWnd)
    {
        edit.setFontId(edit.m_fontId);
        SetWindowSubclass(edit.m_hWnd, decimalEditSubclassProc, 0x65646974 , (DWORD_PTR)&edit);
        setFloatText(edit.m_hWnd, value);
    }
}

GateKeeperEditor::GateKeeperEditor(GateKeeper* gateKeeper)
    : NpcEditor(gateKeeper)
{
    m_title = L"Gate-Keeper";
    m_width = 460;
    m_height = 400;
}

GateKeeperEditor::~GateKeeperEditor()
{
}

void GateKeeperEditor::createExtraControls()
{
    HWND label = CreateWindowExW(0, L"Static", L"Destinations:", 0x50000000, 20, 200, 100, 16, m_hWnd, 0, 0, 0);
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(1), 0);
    label = CreateWindowExW(0, L"Static", L"Name", 0x50000000, 20, 216, 50, 16, m_hWnd, 0, 0, 0);
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(4), 0);
    label = CreateWindowExW(0, L"Static", L"Map ID", 0x50000000, 157, 216, 50, 16, m_hWnd, 0, 0, 0);
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(4), 0);
    label = CreateWindowExW(0, L"Static", L"Pos. X", 0x50000000, 205, 216, 50, 16, m_hWnd, 0, 0, 0);
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(4), 0);
    label = CreateWindowExW(0, L"Static", L"Pos. Y", 0x50000000, 265, 216, 50, 16, m_hWnd, 0, 0, 0);
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(4), 0);
    label = CreateWindowExW(0, L"Static", L"Pos. Z", 0x50000000, 325, 216, 50, 16, m_hWnd, 0, 0, 0);
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(4), 0);
    label = CreateWindowExW(0, L"Static", L"Cost", 0x50000000, 385, 216, 50, 16, m_hWnd, 0, 0, 0);
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(4), 0);

    int y = 236;
    GateKeeper& gateKeeper = dynamic_cast<GateKeeper&>(*m_npc);
    GateTarget* gate = gateKeeper.m_targets;
    UINT id = 210;
    for (int i = 0; i < 3; ++i)
    {
        Destination& row = m_dest[i];
        row.name.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", L"", 0x50010080, 20, y, 130, 18, m_hWnd,
                                          (HMENU)(UINT_PTR)(id - 10), 0, 0);
        if (row.name.m_hWnd)
        {
            row.name.setFontId(row.name.m_fontId);
            SetWindowSubclass(row.name.m_hWnd, tabSubclassProc, 0x65646974 , (DWORD_PTR)&row.name);
        }
        SendMessageW(row.name.m_hWnd, EM_LIMITTEXT, 50, 0);
        row.mapId.EditText::createNumber(m_hWnd, (HMENU)(UINT_PTR)id, 160, y, 30, 18, (UINT)(int)gate->mapId, 0, 0);
        createDecimalEdit(row.x, m_hWnd, 200, y, id + 10, gate->x);
        createDecimalEdit(row.y, m_hWnd, 260, y, id + 20, gate->y);
        createDecimalEdit(row.z, m_hWnd, 320, y, id + 30, gate->z);
        row.cost.EditText::createNumber(m_hWnd, (HMENU)(UINT_PTR)(id + 40), 380, y, 50, 18, (UINT)gate->cost, 0, 0);
        y += 25;
        row.name.setText(gate->name.c_str());
        ++gate;
        ++id;
    }
}

bool GateKeeperEditor::applyExtra()
{
    GateKeeper& gateKeeper = dynamic_cast<GateKeeper&>(*m_npc);
    GateTarget* gate = gateKeeper.m_targets;
    for (int i = 0; i < 3; ++i)
    {
        Destination& row = m_dest[i];
        {
            String text;
            gate->name = *row.name.getText(&text);
        }
        gate->mapId = (int16_t)row.mapId.EditText::getInt(0);
        float value;
        {
            String text;
            row.x.getText(&text);
            value = text.length() ? (float)_wtof(text.c_str()) : 0.0f;
        }
        gate->x = value;
        {
            String text;
            row.y.getText(&text);
            value = text.length() ? (float)_wtof(text.c_str()) : 0.0f;
        }
        gate->y = value;
        {
            String text;
            row.z.getText(&text);
            value = text.length() ? (float)_wtof(text.c_str()) : 0.0f;
        }
        gate->z = value;
        gate->cost = (int)row.cost.EditText::getInt(0);
        ++gate;
    }
    return true;
}

bool GateKeeperEditor::edit(HWND parent, GateKeeper* gateKeeper)
{
    GateKeeperEditor editor(gateKeeper);
    bool ok;
    if (editor.create(parent))
        ok = editor.doModal() == 1;
    else
        ok = false;
    return ok;
}
