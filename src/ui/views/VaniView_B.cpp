#include "VaniView.h"
#include "VaniCtrlsPanel.h"
#include "Vani.h"
#include "Drawable.h"
#include "FileTree.h"
#include "ContentProvider.h"
#include "Bytes.h"
#include "String.hpp"
#include "ida_compat.h"
#include "_imports_p06.h"
#include <shlwapi.h>

bool VaniView::onCreate()
{
    if (!m_data || !PathFindExtensionW(m_data->m_name.m_buffer) || !Document::onCreate())
        return false;
    if (m_data && (m_data->displayV2() & 2))
        setModified(true);
    RECT rc;
    getClientRect(&rc);
    rc.right -= 240;
    if (!m_viewer.createView(m_hWnd, (HMENU)0x65, &rc, 0x50000000, 0))
        return false;
    Drawable* drawable = m_viewer.m_drawable;
    m_viewer.m_renderMode = 1;
    if (drawable && (!drawable->m_texture || drawable->m_texture == (GLuint)-1))
        drawable->loadTexture(0);
    m_viewer.createMouseControler(0);
    rc.left = rc.right;
    rc.right += 240;

    VaniCtrlsPanel* panel = new VaniCtrlsPanel(this, &m_viewer);
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

    Vani* vani = new Vani();
    vani->read(data);
    String folder(m_data->m_path);
    folder.removeFileName();
    folder.stripRight(L'\\');
    folder.removeFileName();
    folder << L"Texture\\";
    vani->loadTexture(folder.m_buffer);
    m_viewer.setDrawable(vani);
    panel->setVani(vani);
    m_drawable = vani;
    return true;
}
