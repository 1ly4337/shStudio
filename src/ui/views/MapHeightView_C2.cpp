#include "MapHeightView.h"
#include "MapHeight.h"
#include "fPoint.h"
#include "String.hpp"
#include "Bytes.h"
#include "glViewer.h"
#include "Drawable.h"
#include "EditText.h"
#include "UpDownCtrl.h"
#include "FlatButton.h"
#include "FileDialogs.h"
#include "gdi_cache.h"
#include "CtrlHelpers.h"
#include "world_common.h"
#include <commctrl.h>
#include <shobjidl.h>
#include <cstdio>

COMDLG_FILTERSPEC g_heightmapSaveFilters[4] = {
    { L"Raw File (Unity 3D)", L"*.raw" },
    { L"Scaled Heights",      L"*.csv" },
    { L"Bitmap File",         L"*.bmp" },
    { L"Texture File",        L"*.txt" },
};

bool MapHeightView::onCreate()
{
    if (!PopupView3D::onCreate())
        return false;

    show(true);
    UpdateWindow(m_hWnd);

    m_doc->computeBounds();

    glViewer& viewer = m_viewer;
    viewer.createMouseControler(0);
    {
        fPoint target((float)(m_doc->m_size - 1), 100.0f, (float)(m_doc->m_size - 1));
        viewer.setPosition(&target);
    }
    viewer.m_renderMode = 0;
    viewer.setNavigationMode(1);
    setModel(m_doc);
    return true;
}

bool MapHeightView::vslot80(int x)
{
    HANDLE bitmap = LoadImageW(hModule, MAKEINTRESOURCEW(0xA9), IMAGE_BITMAP, 0, 0,
                               LR_CREATEDIBSECTION | LR_LOADTRANSPARENT);

    HWND label = CreateWindowExW(0, L"Static", L"X:", WS_CHILD | WS_VISIBLE, x, 5, 10, 15,
                                 m_hStatusBar, 0, 0, 0);
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(4), 0);
    x += 12;
    m_posEdits[0].createNumber(m_hStatusBar, (HMENU)250, x, 3, 60, 17, 0, 0, 0);

    x += 70;
    label = CreateWindowExW(0, L"Static", L"Y:", WS_CHILD | WS_VISIBLE, x, 5, 10, 15,
                            m_hStatusBar, 0, 0, 0);
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(4), 0);
    x += 12;
    m_posEdits[1].createNumber(m_hStatusBar, (HMENU)251, x, 3, 60, 17, 0, 0, 0);

    x += 70;
    label = CreateWindowExW(0, L"Static", L"Z:", WS_CHILD | WS_VISIBLE, x, 5, 10, 15,
                            m_hStatusBar, 0, 0, 0);
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(4), 0);
    x += 12;
    m_posEdits[2].createNumber(m_hStatusBar, (HMENU)252, x, 3, 60, 17, 0, 0, 0);

    x += 70;
    int id = 200;
    for (int i = 0; i < 3; ++i, ++id)
    {
        if (bitmap)
        {
            m_rotReset[i].create(m_hStatusBar, (HMENU)(INT_PTR)id, x, 3, 18,
                                                            18, bitmap, id - 71);
            x += 20;
        }
        EditText& edit = m_rotEdit[i];
        edit.createNumber(m_hStatusBar, (HMENU)(INT_PTR)(id + 10), x, 3, 50, 17, 0, 0, 0);
        WPARAM buddy = (WPARAM)edit.m_hWnd;
        x += 55;
        UpDownCtrl& upDown = m_rotSpin[i];
        upDown.m_hWnd = CreateWindowExW(0, L"msctls_updown32", 0,
                                        WS_CHILD | WS_VISIBLE | UDS_WRAP | UDS_SETBUDDYINT | UDS_ALIGNRIGHT |
                                            UDS_ARROWKEYS | UDS_HORZ,
                                        x, 3, 20, 17, m_hStatusBar, (HMENU)(INT_PTR)(id + 20), 0, 0);
        if (upDown.m_hWnd)
        {
            upDown.setFontId(upDown.m_fontId);
            SendMessageW(upDown.m_hWnd, UDM_SETBUDDY, buddy, 0);
        }
        else
        {
            GetLastError();
        }
        SendMessageW(upDown.m_hWnd, UDM_SETRANGE, 0, 360);
    }

    if (bitmap)
    {
        for (int i = 0, btnId = 230; i < 4; ++i, ++btnId)
        {
            m_posReset[i].create(m_hStatusBar, (HMENU)(INT_PTR)btnId, x, 3,
                                                             18, 18, bitmap, btnId - 98);
            x += 20;
        }
    }
    DeleteObject(bitmap);

    x += 20;
    HWND fpsLabel = CreateWindowExW(0, L"Static", L"fps:", WS_CHILD | WS_VISIBLE, x, 5, 40, 15,
                                    m_hStatusBar, 0, 0, 0);
    if (fpsLabel)
        SendMessageW(fpsLabel, WM_SETFONT, (WPARAM)getStdFont(4), 0);

    HWND fpsValue = CreateWindowExW(0, L"Static", L"", WS_CHILD | WS_VISIBLE, x + 24, 5, 50, 17,
                                    m_hStatusBar, 0, 0, 0);
    if (fpsValue)
        SendMessageW(fpsValue, WM_SETFONT, (WPARAM)getStdFont(4), 0);

    m_fpsLabel = fpsValue;
    SetTimer(m_hWnd, 0x86, 250, 0);
    return true;
}

void MapHeightView::vslot3(const fPoint* pos)
{
    wchar_t buffer[32];

    m_updating = true;
    HWND edit = m_posEdits[0].m_hWnd;
    swprintf_s(buffer, 32, L"%.0f", (double)pos->x);
    SendMessageW(edit, WM_SETTEXT, 0, (LPARAM)buffer);

    edit = m_posEdits[1].m_hWnd;
    swprintf_s(buffer, 32, L"%.2f", (double)pos->y);
    SendMessageW(edit, WM_SETTEXT, 0, (LPARAM)buffer);

    edit = m_posEdits[2].m_hWnd;
    swprintf_s(buffer, 32, L"%.0f", (double)pos->z);
    SendMessageW(edit, WM_SETTEXT, 0, (LPARAM)buffer);
    m_updating = false;
}

static inline void MapHeightView_checkMenuItem(HMENU menu, UINT id, bool checked)
{
    MENUITEMINFOW mii;
    memset(&mii.fMask, 0, sizeof(mii) - sizeof(mii.cbSize));
    mii.cbSize = sizeof(MENUITEMINFOW);
    mii.fMask = MIIM_STATE;
    GetMenuItemInfoW(menu, id, FALSE, &mii);
    if ((mii.fState & (MFS_GRAYED | MFS_DISABLED)) == 0)
    {
        mii.fState = checked ? MFS_CHECKED : 0;
        SetMenuItemInfoW(menu, id, FALSE, &mii);
    }
}

bool MapHeightView::onCommand(UINT code, HWND ctrl, UINT id)
{
    if (code == EN_UPDATE)
    {
        UINT axis = id - 250;
        if (axis <= 2)
        {
            if (!m_updating)
            {
                glViewer& viewer = m_viewer;
                fPoint pos(viewer.m_position.x, viewer.m_position.y, viewer.m_position.z);
                float value = m_posEdits[axis].getFloat();
                pos.at(axis) = value;
                viewer.setPosition(&pos);
            }
            return true;
        }
    }

    if ((code | 1) != 1 || ctrl)
        return PopupView3D::onCommand(code, ctrl, id);

    if (id == 0xE104)
    {
        String fileName;
        DWORD filterIndex = 1;
        if (FileDialogs_save(m_hWnd, &fileName, L".raw", g_heightmapSaveFilters, 4, &filterIndex,
                             L"Save heightmap as") == 0)
        {
            if (filterIndex == 1)
            {
                Bytes bytes(0);
                static_cast<ToggleNode*>(m_doc)->write(bytes, 'raw ');
                bytes.aStream::save(fileName.m_buffer);
            }
            else if (filterIndex == 2)
            {
                String text;
                text.m_flags |= 0x10;
                for (int row = 0; row < m_doc->m_size; ++row)
                {
                    for (int col = 0; col < m_doc->m_size; ++col)
                        (text << (unsigned long)(int)m_doc->m_grid[row + col * m_doc->m_size].y) << L'\t';
                    for (int i = 0; i < 1; ++i)
                    {
                        if (!text.m_length)
                            break;
                        --text.m_length;
                        text.m_buffer[text.m_length] = 0;
                    }
                    text << L"\r\n";
                }

                FILE* stream = 0;
                _wfopen_s(&stream, fileName.m_buffer, L"wb");
                if (stream)
                {
                    if (text.m_length)
                    {
                        int bom = 0xFEFF;
                        fwrite(&bom, 2, 1, stream);
                        fwrite(text.m_buffer, 2 * text.m_length, 1, stream);
                    }
                    fflush(stream);
                    fclose(stream);
                }
            }
            else if (filterIndex == 3)
            {
                int n = m_doc->m_size - 1;
                int dims[2] = { n, n };
                Bytes bytes(0);
                if (!writeGrayBmpHeader(dims, &bytes))
                {
                    MapHeight* doc = m_doc;
                    float range = (m_doc->m_max.y - m_doc->m_min.y) * 1.1f;
                    unsigned char* line = new unsigned char[n];
                    for (int row = n - 1; row >= 0; --row)
                    {
                        for (int col = 0; col < n; ++col)
                        {
                            MapHeight::GridPoint p = m_doc->m_grid[row * n + col];
                            line[col] = (unsigned char)(int)((double)((p.y - doc->m_min.y) / range) * 256.0);
                        }
                        bytes.put(line, n);
                    }
                    delete[] line;
                    bytes.aStream::save(fileName.m_buffer);
                }
            }
            else if (filterIndex == 4)
            {
                String text;
                const BYTE* texture = m_doc->m_texIndex;
                int row = 0;
                for (row = 0; row < m_doc->m_size; ++row)
                {
                    for (int col = 0; col < m_doc->m_size; ++col, ++texture)
                        (text << (unsigned char)*texture) << ',';
                    for (int i = 0; i < 1; ++i)
                    {
                        if (!text.m_length)
                            break;
                        --text.m_length;
                        text.m_buffer[text.m_length] = 0;
                    }
                    text << L"\r\n";
                }
                text.saveAs(fileName.m_buffer, row, 1200 );
            }
        }
        return true;
    }

    switch (id)
    {
    case 0xE165:
        m_editMode = true;
        m_doc->m_drawMode = 0;
        m_viewer.m_renderMode = 0;
        return true;

    case 0xE166:
    case 0xE167:
    {
        m_editMode = false;
        SetCursor(hCursor);
        bool ok;
        if (id == 0xE166)
        {
            ok = m_doc->loadTextures();
            if (ok)
                m_doc->m_drawMode = 1;
        }
        else
        {
            ok = m_doc->loadMiniMap();
            if (ok)
                m_doc->m_drawMode = 2;
        }
        if (ok)
        {
            glViewer& viewer = m_viewer;
            Drawable* drawable = viewer.m_drawable;
            viewer.m_renderMode = 1;
            if (drawable && (drawable->m_texture == 0 || drawable->m_texture == (GLuint)-1))
                drawable->loadTexture(0);
        }
        SetCursor(g_arrowCursor);
        invalidate(true);
        return true;
    }

    case 0xE168:
        m_doc->m_followEye = !m_doc->m_followEye;
        return true;
    }
    return PopupView3D::onCommand(code, ctrl, id);
}

bool MapHeightView::onInitMenu(HMENU menu)
{
    bool editMode = m_editMode;
    MapHeightView_checkMenuItem(menu, 0xE165, editMode);
    MapHeightView_checkMenuItem(menu, 0xE166, !m_editMode);
    MapHeightView_checkMenuItem(menu, 0xE168, m_doc->m_followEye);
    return true;
}

bool MapHeightView::onTimer(UINT_PTR id)
{
    if (id == 0x86)
    {
        float fps = m_doc->m_fps;
        if (fps > 0.0f && 1000.0f > fps)
            setWindowDouble(m_fpsLabel, L"%.0f", (double)fps);
        SetTimer(m_hWnd, 0x86, 1000, 0);
        return false;
    }
    if (id == 0x82)
    {
        SetForegroundWindow(m_hWnd);
        UpdateWindow(m_hWnd);
        return true;
    }
    return false;
}
