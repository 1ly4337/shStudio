#include "ItmEditor.h"
#include "ItemModel.h"
#include "StudioFrame.h"
#include "FileDialogs.h"
#include "gdi_cache.h"
#include "CtrlHelpers.h"
#include "Window.h"
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <shlwapi.h>
#include "_imports_p06.h"

namespace {

void createLabel(HWND parent, LPCWSTR text, int x, int y, int cx, int cy)
{
    HWND label = CreateWindowExW(0, L"Static", text, 0x50000000, x, y, cx, cy, parent, 0, 0, 0);
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(4), 0);
}

bool createEdit(EditText& edit, HWND parent, LPCWSTR text, int x, int y, int cx, int cy, HMENU id,
                SUBCLASSPROC proc)
{
    edit.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", text, 0x50010080, x, y, cx, cy, parent, id, 0, 0);
    if (!edit.m_hWnd)
        return false;
    edit.setFontId(edit.m_fontId);
    SetWindowSubclass(edit.m_hWnd, proc, 'edit', (DWORD_PTR)&edit);
    return true;
}

void createBrowseButton(FlatButton& button, HWND parent, HMENU id, int x, int y)
{
    HBITMAP bitmap = (HBITMAP)LoadImageW(GetModuleHandleW(0), MAKEINTRESOURCEW(0xA8), IMAGE_BITMAP, 0, 0,
                                         LR_CREATEDIBSECTION);
    button.m_bmpNormal = bitmap;
    if (bitmap)
    {
        BITMAP bm;
        GetObjectW(bitmap, sizeof(BITMAP), &bm);
        button.createWindow(parent, id, x, y, bm.bmWidth, bm.bmHeight);
    }
}

void addComboItem(ComboBox& combo, const wchar_t* text)
{
    if (SendMessageW(combo.m_hWnd, CB_ADDSTRING, 0, (LPARAM)text) != CB_ERR)
    {
        LRESULT count = SendMessageW(combo.m_hWnd, CB_GETCOUNT, 0, 0);
        combo.m_items.StringTable::put((int)count, text);
    }
}

float readFloat(EditText& edit)
{
    String text;
    edit.getText(&text);
    float value = 0.0f;
    if (text.length())
        value = (float)_wtof(text.c_str());
    return value;
}

}

ItmEditor::ItmEditor(ItemModel* model, const wchar_t* basePath)
    : Modal()
    , m_model(model)
{
    m_basePath.append(basePath, (unsigned)-1);
}

ItmEditor::~ItmEditor()
{
}

bool ItmEditor::create(HWND parent)
{
    m_readOnly = g_studioFrame->isArchiveReadOnly();
    m_buttons = 1;
    RECT rc;
    SetRect(&rc, CW_USEDEFAULT, CW_USEDEFAULT, 300, 270);
    if (!Window::create(parent, 0, &rc, 0x80C80000, 0x10101, L"ITM Editor"))
        return false;
    HWND top = Modal_getTopLevelWindow(parent);
    m_owner = top;
    Window_centerOn(m_hWnd, top);
    show(true);
    return true;
}

bool ItmEditor::onCreate()
{
    createLabel(m_hWnd, L"DDS file:", 20, 22, 60, 16);
    createEdit(m_texture, m_hWnd, m_model->m_texture.c_str(), 80, 20, 160, 18, (HMENU)'dds ', tabSubclassProc);
    SendMessageW(m_texture.m_hWnd, EM_LIMITTEXT, 64, 0);
    if (!m_readOnly)
        createBrowseButton(m_browseTexture, m_hWnd, (HMENU)0x66, 250, 20);

    createLabel(m_hWnd, L"3DO file:", 20, 46, 60, 16);
    createEdit(m_vertex, m_hWnd, m_model->m_vertex.c_str(), 80, 44, 160, 18, (HMENU)'3dc ', tabSubclassProc);
    SendMessageW(m_vertex.m_hWnd, EM_LIMITTEXT, 64, 0);
    if (!m_readOnly)
        createBrowseButton(m_browseVertex, m_hWnd, (HMENU)0x68, 250, 44);

    createLabel(m_hWnd, L"Glow:", 20, 73, 40, 16);
    m_glow.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"ComboBox", 0, (m_glow.m_dropLines & 3) | 0x50010000,
                                    80, 68, 160, 250, m_hWnd, (HMENU)'glow', 0, 0);
    if (m_glow.m_hWnd)
        m_glow.setFontId(m_glow.m_fontId);
    else
        GetLastError();
    addComboItem(m_glow, L"none");
    addComboItem(m_glow, L"blue");
    addComboItem(m_glow, L"red");
    addComboItem(m_glow, L"green");
    addComboItem(m_glow, L"white");
    SendMessageW(m_glow.m_hWnd, CB_SETCURSEL, m_model->m_glow + 1, 0);

    createLabel(m_hWnd, L"Parameters:", 20, 100, 60, 16);
    m_unk2.createNumber(m_hWnd, (HMENU)'unk2', 80, 98, 50, 18, m_model->m_unk2, 0, 0);
    createLabel(m_hWnd, L"-", 135, 100, 10, 18);
    m_unk3.createNumber(m_hWnd, (HMENU)'unk3', 145, 98, 50, 18, m_model->m_unk3, 0, 0);

    bool extended = m_model->m_format != 0;
    createLabel(m_hWnd, L"Format:", 20, 124, 60, 16);
    m_extended.m_hWnd = CreateWindowExW(0, L"Button", L" Extended", 0x50000002, 80, 122, 80, 18, m_hWnd,
                                        (HMENU)0x6E, 0, 0);
    if (m_extended.m_hWnd)
        m_extended.setFontId(m_extended.m_fontId);
    else
        GetLastError();
    if (m_extended.m_hWnd)
        SetWindowSubclass(m_extended.m_hWnd, noEraseSubclassProc, 'chck', (DWORD_PTR)&m_extended);
    m_extended.setCheck(extended);

    createLabel(m_hWnd, L"Rotation:", 20, 148, 60, 16);
    if (createEdit(m_rotation, m_hWnd, L"", 80, 146, 50, 18, (HMENU)'posX', decimalEditSubclassProc))
        m_rotation.setFloat(m_model->m_rotY);
    createLabel(m_hWnd, L"Scaling:", 145, 148, 60, 16);
    if (createEdit(m_scale, m_hWnd, L"", 205, 146, 50, 18, (HMENU)'posY', decimalEditSubclassProc))
        m_scale.setFloat(m_model->m_scale);

    createLabel(m_hWnd, L"Color:", 20, 172, 60, 16);
    m_color.create(m_hWnd, (HMENU)0x636C6F72 , 80, 170, 50, 18, m_model->m_rgba, 0, 0);
    createLabel(m_hWnd, L"Unknown:", 145, 172, 60, 16);
    m_unk5.createNumber(m_hWnd, (HMENU)'posZ', 205, 170, 50, 18, m_model->m_unk5, 0, 0);

    m_color.enable(extended);
    m_rotation.enable(extended);
    m_scale.enable(extended);
    m_unk5.enable(extended);

    if (!m_readOnly)
    {
        m_btnWidth = 60;
        RECT rcBuf;
        RECT* rc = getClientRect(&rcBuf);
        m_rename.m_hWnd = CreateWindowExW(0, L"Button", L"Rename Files", 0x50000000, 18, rc->bottom - 40, 80, 24,
                                          m_hWnd, (HMENU)0x78, 0, 0);
        if (m_rename.m_hWnd)
            m_rename.setFontId(m_rename.m_fontId);
        else
            GetLastError();
    }
    return Modal::onCreate();
}

bool ItmEditor::onCommand(UINT code, HWND ctrl, UINT id)
{
    if (!m_readOnly && (id == 0x66 || id == 0x68))
    {
        String path(m_basePath);
        const wchar_t* filter;
        if (id == 0x66)
        {
            filter = L"DDS Files\0*.dds\0";
            path << L"DDS\\";
        }
        else
        {
            filter = L"3DO Files\0*.3DO\0";
            path << L"3DO\\";
        }
        if (FileDialogs_open(m_hWnd, &path, filter, 0, true))
        {
            LPWSTR name = PathFindFileNameW(path.c_str());
            if (id == 0x66)
                m_texture.setText(name);
            else
                m_vertex.setText(name);
        }
        return true;
    }
    if (id == 0x6E)
    {
        bool extended = !m_extended.isChecked();
        m_extended.setCheck(extended);
        m_color.enable(extended);
        m_rotation.enable(extended);
        m_scale.enable(extended);
        m_unk5.enable(extended);
        return true;
    }
    if (id == 0x78)
    {
        String name;
        m_texture.getText(&name);
        if (name.endsWithNoCase(L".dds"))
        {
            String newPath = (m_basePath + L"DDS\\") + name;
            String oldPath = (m_basePath + L"DDS\\") + m_model->m_texture;
            _wrename(oldPath.c_str(), newPath.c_str());
            EnableWindow(m_btnCancel, FALSE);
        }
        {
            String text;
            name = *m_vertex.getText(&text);
        }
        if (name.endsWithNoCase(L".3do"))
        {
            String newPath = (m_basePath + L"3DO\\") + name;
            String oldPath = (m_basePath + L"3DO\\") + m_model->m_vertex;
            _wrename(oldPath.c_str(), newPath.c_str());
            EnableWindow(m_btnCancel, FALSE);
        }
        return true;
    }
    return Modal::onCommand(code, ctrl, id);
}

int ItmEditor::mapCommandToResult(int id)
{
    int result = id;
    if (id != IDOK)
        return result;
    {
        String text;
        m_model->m_texture = *m_texture.getText(&text);
    }
    {
        String text;
        m_model->m_vertex = *m_vertex.getText(&text);
    }
    if (m_model->m_texture.length())
    {
        String ext = m_model->m_texture.getExtension();
        if (ext.length())
        {
            if (ext.String::sub_4121F0(L".dds"))
                result = -1;
        }
        else
        {
            m_model->m_texture << L".dds";
        }
    }
    else
    {
        result = -1;
    }
    if (m_model->m_vertex.length())
    {
        String ext = m_model->m_vertex.getExtension();
        if (ext.length())
        {
            if (ext.String::sub_4121F0(L".3do"))
                result = -1;
        }
        else
        {
            m_model->m_vertex << L".3do";
        }
    }
    else
    {
        result = -1;
    }
    m_model->m_glow = (int)SendMessageW(m_glow.m_hWnd, CB_GETCURSEL, 0, 0) - 1;
    m_model->m_unk2 = m_unk2.getInt(0);
    m_model->m_unk3 = m_unk3.getInt(0);
    m_model->m_format = m_extended.isChecked() != 0;
    m_model->m_rgba = m_color.m_color;
    m_model->m_rotY = readFloat(m_rotation);
    m_model->m_scale = readFloat(m_scale);
    m_model->m_unk5 = m_unk5.getInt(-1);
    return result;
}
