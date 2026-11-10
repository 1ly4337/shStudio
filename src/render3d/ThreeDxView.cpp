#include "ThreeDxView.h"
#include "ThreeDxCtrlsPanel.h"
#include "View3DInfo.h"
#include "Model3D.h"
#include "Drawable.h"
#include "FileTree.h"
#include "Preferences.h"
#include "ContentProvider.h"
#include "Clipboard.h"
#include "menus.h"
#include "StudioFrame.h"
#include "Bytes.h"
#include "oStream.hpp"
#include "fPoint.h"
#include "clipboard_A.h"
#include "ida_compat.h"
#include "_imports_p06.h"
#include <shlwapi.h>

ThreeDxView::~ThreeDxView()
{
    if (m_panel)
        delete m_panel;
}

bool ThreeDxView::onCreate()
{
    if (!m_data)
        return false;
    LPWSTR ext = PathFindExtensionW(m_data->m_name.m_buffer);
    if (!ext)
        return false;
    String extension;
    extension.append(ext, (unsigned)-1);
    extension.toLowerInPlace();
    DWORD type;
    if (extension == L".3do")
        type = 'k3DO';
    else if (extension == L".3dc")
        type = 'k3DC';
    else if (extension == L".3de")
        type = 'k3DE';
    else
        return false;
    if (!Document::onCreate())
        return false;
    if (m_data && (m_data->displayV2() & 2))
        setModified(true);

    RECT rc;
    getClientRect(&rc);
    rc.right -= 240;
    if (!m_viewer.createView(m_hWnd, (HMENU)0x65, &rc, 0x50000000, 0))
        return false;
    m_viewer.m_renderMode = 0;
    m_viewer.createMouseControler(0);
    m_viewer.setDrawFlag(0, g_preferences.getInt(L"3D View", L"Transparency", 0) == 1);
    rc.left = rc.right;
    rc.right += 240;

    ThreeDxCtrlsPanel* panel = new ThreeDxCtrlsPanel(this, &m_viewer);
    m_panel = panel;
    m_viewer.m_controls = panel ? static_cast<glControls*>(panel) : 0;
    if (!panel->createChild(m_hWnd, (HMENU)0x66, &rc, 0x50000000, 0, 0))
        return false;

    Bytes data(0);
    int err;
    if (g_contentProvider)
        err = g_contentProvider->readFile(m_data, &data, false);
    else
        err = data.aStream::load(m_data->m_path.m_buffer);
    if (err)
        return false;

    Model3D* model = new Model3D(type);
    static_cast<Streamable*>(model)->read(data, 'file');
    m_drawable = model;
    m_viewer.setDrawable(model);

    m_textureDir = m_data->m_path;
    m_textureDir.removeFileName();
    m_textureDir.stripRight(L'\\');
    m_textureDir.removeFileName();
    m_textureDir << "DDS\\";

    String texture(model->m_textureName);
    if (!texture.m_length) {
        texture = m_data->m_name;
        texture.removeExtension();
        texture << ".dds";
    }
    panel->m_texture.setText(texture.m_buffer);

    bool found;
    if (g_contentProvider)
        found = g_contentProvider->readPath((m_textureDir + texture).m_buffer, &data) == 0;
    else
        found = data.aStream::load((m_textureDir + texture).m_buffer) == 0;
    Drawable* drawable;
    if (found) {
        panel->m_texCheck.setCheck(true);
        model->m_textureName = m_textureDir + texture;
        drawable = m_viewer.m_drawable;
        m_viewer.m_renderMode = 1;
    } else {
        m_textureDir = m_data->m_path;
        m_textureDir.removeFileName();
        if (g_contentProvider)
            found = g_contentProvider->readPath((m_textureDir + texture).m_buffer, &data) == 0;
        else
            found = data.aStream::load((m_textureDir + texture).m_buffer) == 0;
        if (!found)
            goto noTexture;
        panel->m_texCheck.setCheck(true);
        model->m_textureName = m_textureDir + texture;
        drawable = m_viewer.m_drawable;
        m_viewer.m_renderMode = 1;
    }
    if (drawable && (!drawable->m_texture || drawable->m_texture == (GLuint)-1))
        drawable->loadTexture(0);
noTexture:
    {
        fPoint position(0.0f, 0.0f, -2.0f);
        panel->setPosition(&position);
    }
    panel->m_list.m_faces = &model->m_faces;
    panel->m_list.setItemCount(model->m_faces.size());
    return true;
}

void ThreeDxView::onActivate(bool active)
{
    if (active) {
        g_activeGlView = &m_viewer;
        m_viewer.activate(true);
        SetForegroundWindow(m_viewer.m_hWnd);
        SetActiveWindow(m_viewer.m_hWnd);
        SetFocus(m_viewer.m_hWnd);
    } else {
        g_activeGlView = 0;
        m_viewer.activate(false);
    }
}

void ThreeDxView::onSize(WPARAM , int cx, int cy)
{
    int height = cy;
    if (m_hStatusBar) {
        int statusHeight = m_statusHeight;
        height = cy - statusHeight;
        SetWindowPos(m_hStatusBar, 0, 0, cy - statusHeight, cx, statusHeight, SWP_NOZORDER);
    }
    SetWindowPos(m_viewer.m_hWnd, 0, 0, 0, cx - 240, height, SWP_NOMOVE | SWP_NOZORDER);
    if (m_panel)
        SetWindowPos(m_panel->m_hWnd, 0, cx - 240, 0, 240, height, SWP_NOZORDER);
}

static inline void setItemEnabled(HMENU menu, UINT id, bool enabled)
{
    MENUITEMINFOW mii;
    memset(&mii.fMask, 0, 0x2C);
    mii.fState = enabled ? 0 : MFS_GRAYED;
    mii.cbSize = sizeof(mii);
    mii.fMask = MIIM_STATE;
    SetMenuItemInfoW(menu, id, FALSE, &mii);
}

static inline void setItemChecked(HMENU menu, UINT id, bool checked)
{
    MENUITEMINFOW mii;
    memset(&mii.fMask, 0, 0x2C);
    mii.cbSize = sizeof(mii);
    mii.fMask = MIIM_STATE;
    GetMenuItemInfoW(menu, id, FALSE, &mii);
    if ((mii.fState & 3) == 0) {
        mii.fState = checked ? MFS_CHECKED : 0;
        SetMenuItemInfoW(menu, id, FALSE, &mii);
    }
}

bool ThreeDxView::onInitMenu(HMENU menu)
{
    setStatusText(0, 0);
    setItemEnabled(menu, 0xE401, m_type == 'k3DE');
    setItemEnabled(menu, 0xE402, m_type == 'k3DO');
    setItemChecked(menu, 0xE401, m_type == 'k3DE');
    setItemChecked(menu, 0xE402, m_type == 'k3DO');
    setItemEnabled(menu, 0xE403, m_type == 'k3DC');
    setItemEnabled(menu, 0xE404, m_type == 'k3DC');
    setItemEnabled(menu, 0xE405, m_type == 'kVAn');
    Model3D* model = dynamic_cast<Model3D*>(m_drawable);
    if (m_type == 'k3DC' && model) {
        int version = static_cast<Serializable*>(model)->serialV5() & 0xFFF;
        checkMenuItemIfEnabled(menu, 0xE403, (unsigned)version < 0x600);
        checkMenuItemIfEnabled(menu, 0xE404, version >= 0x600);
    }
    return true;
}

bool ThreeDxView::onCommand(UINT code, HWND ctrl, UINT id)
{
    if ((code | 1) != 1)
        return Document::onCommand(code, ctrl, id);
    if (id == 0xE11E) {
        View3DInfo info(m_drawable);
        if (info.vslot78(m_hWnd))
            info.doModal();
        return true;
    }
    Model3D* model = dynamic_cast<Model3D*>(m_drawable);
    switch (id) {
    case 0xE123:
        if (model) {
            String text;
            text.m_flags |= 5;
            static_cast<Streamable*>(model)->toText(text, 'type');
            Clipboard::setText(text.m_buffer, true);
            HBITMAP bitmap = captureClientArea(m_viewer.m_hWnd);
            if (bitmap) {
                clipboard_putBitmap(bitmap);
                DeleteObject(bitmap);
            }
        }
        return true;
    case 0xE403:
        if (model && model->m_type == 'k3DC') {
            static_cast<Serializable*>(model)->setCodePage(0x500);
            setModified(true);
        }
        return true;
    case 0xE404:
        if (model && model->m_type == 'k3DC') {
            static_cast<Serializable*>(model)->setCodePage(0x600);
            setModified(true);
        }
        return true;
    }
    return Document::onCommand(code, ctrl, id);
}

HRESULT ThreeDxView::store()
{
    Model3D* model = dynamic_cast<Model3D*>(m_drawable);
    if (!model)
        return E_POINTER;
    oStream out(false, m_data->m_codePage);
    static_cast<Streamable*>(model)->write(out, 'file');
    HRESULT hr = studioSaveFile(&out, &m_data->m_path);
    return hr;
}
