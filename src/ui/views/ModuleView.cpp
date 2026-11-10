#include "ModuleView.h"
#include "SModCtrlsPanel.h"
#include "SuperModule.h"
#include "View3DInfo.h"
#include "ScaleEditor.h"
#include "Drawable.h"
#include "fPoint.h"
#include "iStream.hpp"
#include "oStream.hpp"
#include "String.hpp"
#include "ContentProvider.h"
#include "Preferences.h"
#include "StudioFrame.h"
#include "clipboard_A.h"
#include "Clipboard.h"
#include "Application.h"
#include "_imports_p06.h"

ModuleView::ModuleView(FileTree* data)
    : Document(data), m_viewer()
{
    m_model = 0;
    m_panel = 0;
}

ModuleView::~ModuleView()
{
    if (m_panel)
        delete m_panel;
}

bool ModuleView::onCreate()
{
    if (!g_studioFrame || !Popup::onCreate())
        return false;
    FileTree* data = m_data;
    if (data && (data->displayV2() & 2) != 0)
        setModified(true);
    int width = getPanelWidth();
    RECT rc;
    getClientRect(&rc);
    rc.right -= width;
    if (!m_viewer.createView(m_hWnd, (HMENU)101, &rc, 0x50000000, 0))
        return false;
    rc.left = rc.right;
    rc.right += width;
    m_panel = createPanel();
    if (!m_panel || !m_panel->createChild(m_hWnd, (HMENU)102, &rc, 0x50000000, 0, 0))
        return false;
    m_viewer.m_controls = m_panel ? static_cast<glControls*>(m_panel) : 0;
    int transparency = g_preferences.getInt(L"3D View", L"Transparency", 0);
    HWND hwnd = m_hWnd;
    m_viewer.m_drawFlags = transparency == 1;
    SetTimer(hwnd, 0x84, 0xFA, 0);
    return true;
}

HRESULT ModuleView::store()
{
    if (!m_model)
        return E_POINTER;
    oStream out(false, FIELD(UINT, m_ownedData, 0x140));
    Streamable* streamable = dynamic_cast<Streamable*>(m_model);
    if (!streamable)
        throw 0;
    streamable->write(out, 'file');
    return studioSaveStream(&out, m_data, 0);
}

bool ModuleView::loadModel()
{
    FileTree* data = m_data;
    if (!data)
        return false;
    iStream in(false, FIELD(UINT, data, 0x140));
    int error;
    if (g_contentProvider)
        error = g_contentProvider->readFile(data, &in, false);
    else
        error = in.load(FIELD(String, data, 0x10).m_buffer);
    if (error || (m_model = createModel()) == 0)
        return false;
    Streamable* streamable = dynamic_cast<Streamable*>(m_model);
    if (!streamable)
        throw 0;
    DWORD fmt = 'file';
    if (FIELD(String, m_data, 0x10).endsWithNoCase(L"dg_pv"))
        fmt = 'dgpv';
    streamable->read(in, fmt);
    if (g_contentProvider)
    {
        m_model->loadTexture(0);
    }
    else
    {
        String dir(FIELD(String, m_data, 0x10));
        dir.removeFileName();
        dir.toParentPath();
        dir << "Texture\\";
        m_model->loadTexture(dir.m_buffer);
    }
    m_viewer.setDrawable(m_model);
    return true;
}

bool ModuleView::onTimer(UINT_PTR id)
{
    if (id != 0x84)
        return false;
    if (!loadModel())
    {
        String message;
        FileTree* data = m_data;
        message << L"The file ";
        const String& name = FIELD(String, data, 0x30);
        message.append(name.m_buffer, name.m_length);
        message << L" can not be processed.";
        setStatusText(message.m_buffer, 0);
    }
    return true;
}

Drawable* ModuleView::createModel()
{
    return new SuperModule();
}

void ModuleView::onActivate(bool active)
{
    if (!active)
    {
        m_viewer.activate(false);
        return;
    }
    g_activeGlView = &m_viewer;
    m_viewer.activate(true);
    SetForegroundWindow(m_viewer.m_hWnd);
    SetActiveWindow(m_viewer.m_hWnd);
    SetFocus(m_viewer.m_hWnd);
}

bool ModuleView::onClose()
{
    g_activeGlView = 0;
    return Doc::onClose();
}

void ModuleView::onSize(WPARAM type, int cx, int cy)
{
    HWND status = m_hStatusBar;
    int height = cy;
    if (status)
    {
        int statusHeight = m_statusHeight;
        height = cy - statusHeight;
        SetWindowPos(status, 0, 0, cy - statusHeight, cx, statusHeight, SWP_NOZORDER);
    }
    int width = getPanelWidth();
    int viewWidth = cx - width;
    SetWindowPos(m_viewer.m_hWnd, 0, 0, 0, viewWidth, height, SWP_NOZORDER | SWP_NOMOVE);
    SetWindowPos(m_panel->m_hWnd, 0, viewWidth, 0, width, height, SWP_NOZORDER);
}

bool ModuleView::onCommand(UINT code, HWND ctrl, UINT id)
{
    if ((code | 1) != 1)
        return Doc::onCommand(code, ctrl, id);
    if (id == 0xE11E)
    {
        View3DInfo info(m_model);
        if (info.vslot78(m_hWnd))
            info.doModal();
        return true;
    }
    if (id == 0xE123)
    {
        Streamable* streamable = dynamic_cast<Streamable*>(m_model);
        if (!streamable)
            return true;
        String text;
        text.m_flags |= 5;
        streamable->toText(text, 'code');
        Clipboard::setText(text.m_buffer, true);
        HBITMAP bitmap = captureClientArea(m_viewer.m_hWnd);
        if (bitmap)
        {
            clipboard_putBitmap(bitmap);
            DeleteObject(bitmap);
        }
        return true;
    }
    if (id == 0xE413)
    {
        fPoint factors(1.0f, 1.0f, 1.0f);
        ScaleEditor editor(&factors);
        if (editor.create(m_hWnd) && editor.doModal() == 1)
        {
            Drawable* model = m_model;
            if (model)
            {
                model->scale(&factors);
                setModified(true);
            }
        }
    }
    return Doc::onCommand(code, ctrl, id);
}

bool ModuleView::onKey(WPARAM vk, LPARAM flags, bool down)
{
    if (GetKeyState(VK_CONTROL) < 0 && (short)vk == 'C')
    {
        onCommand(0, 0, 0xE123);
        return true;
    }
    if (GetKeyState(VK_CONTROL) < 0 && (short)vk == 'W')
    {
        onCommand(0, 0, 0xE102);
        return true;
    }
    return false;
}

CtrlsPanel* ModuleView::createPanel()
{
    return new SModCtrlsPanel(this, &m_viewer);
}

int ModuleView::getPanelWidth()
{
    return 120;
}
