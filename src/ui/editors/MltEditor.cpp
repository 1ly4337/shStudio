#include "MltEditor.h"
#include "ModelMLT.h"
#include "StudioFrame.h"
#include "FileDialogs.h"
#include "gdi_cache.h"
#include "CtrlHelpers.h"
#include "Window.h"
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

void createEdit(EditText& edit, HWND parent, LPCWSTR text, int x, int y, int cx, int cy, HMENU id)
{
    edit.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", text, 0x50010080, x, y, cx, cy, parent, id, 0, 0);
    if (edit.m_hWnd)
    {
        edit.setFontId(edit.m_fontId);
        SetWindowSubclass(edit.m_hWnd, tabSubclassProc, 'edit', (DWORD_PTR)&edit);
    }
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

}

MltEditor::MltEditor(ModelMLT* model, const wchar_t* basePath)
    : Modal()
    , m_model(model)
{
    m_basePath.append(basePath, (unsigned)-1);
}

MltEditor::~MltEditor()
{
}

bool MltEditor::create(HWND parent)
{
    m_readOnly = g_studioFrame->isArchiveReadOnly();
    m_buttons = 1;
    RECT rc;
    SetRect(&rc, CW_USEDEFAULT, CW_USEDEFAULT, 350, 200);
    if (!Window::create(parent, 0, &rc, 0x80C80000, 0x10101, L"MLT Editor"))
        return false;
    HWND top = Modal_getTopLevelWindow(parent);
    m_owner = top;
    Window_centerOn(m_hWnd, top);
    show(true);
    return true;
}

bool MltEditor::onCreate()
{
    createLabel(m_hWnd, L"DDS File:", 20, 22, 70, 16);
    createEdit(m_texture, m_hWnd, m_model->m_texture.c_str(), 100, 20, 180, 18, (HMENU)'dds ');
    SendMessageW(m_texture.m_hWnd, EM_LIMITTEXT, 64, 0);
    if (!m_readOnly)
        createBrowseButton(m_browseTexture, m_hWnd, (HMENU)0x66, 290, 20);

    createLabel(m_hWnd, L"3DC File:", 20, 48, 70, 16);
    createEdit(m_vertex, m_hWnd, m_model->m_vertex.c_str(), 100, 46, 180, 18, (HMENU)'3dc ');
    SendMessageW(m_vertex.m_hWnd, EM_LIMITTEXT, 64, 0);
    if (!m_readOnly)
        createBrowseButton(m_browseVertex, m_hWnd, (HMENU)0x68, 290, 46);

    createLabel(m_hWnd, L"Alpha Channel:", 20, 79, 80, 16);
    m_alpha.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"ComboBox", 0, (m_alpha.m_dropLines & 3) | 0x50010000,
                                     100, 74, 100, 250, m_hWnd, (HMENU)'alph', 0, 0);
    if (m_alpha.m_hWnd)
        m_alpha.setFontId(m_alpha.m_fontId);
    else
        GetLastError();
    addComboItem(m_alpha, L"Visibility");
    addComboItem(m_alpha, L"Glow");
    SendMessageW(m_alpha.m_hWnd, CB_SETCURSEL, m_model->m_alpha, 0);

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

bool MltEditor::onCommand(UINT code, HWND ctrl, UINT id)
{
    if (m_readOnly)
        return Modal::onCommand(code, ctrl, id);
    if (id == 0x66 || id == 0x68)
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
            filter = L"3DC Files\0*.3DC\0";
            path << L"3DC\\";
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

int MltEditor::mapCommandToResult(int id)
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
    m_model->m_alpha = (int)SendMessageW(m_alpha.m_hWnd, CB_GETCURSEL, 0, 0);
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
            if (ext.String::sub_4121F0(L".3dc"))
                result = -1;
        }
        else
        {
            m_model->m_vertex << L".3dc";
        }
    }
    else
    {
        result = -1;
    }
    return result;
}
