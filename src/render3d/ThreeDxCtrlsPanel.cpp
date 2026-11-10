#include "ThreeDxCtrlsPanel.h"
#include "ThreeDxView.h"
#include "Doc.h"
#include "glViewer.h"
#include "Model3D.h"
#include "Face.h"
#include "Drawable.h"
#include "Preferences.h"
#include "FileDialogs.h"
#include "String.hpp"
#include "Exception.h"
#include "ida_compat.h"
#include "_imports_p06.h"
#include <commctrl.h>
#include <cstdio>
#include <cstring>

wchar_t g_facetText[16];

static inline HWND createLabel(HWND parent, LPCWSTR text, int x, int y, int cx, int cy)
{
    HWND hwnd = CreateWindowExW(0, L"Static", text, WS_CHILD | WS_VISIBLE, x, y, cx, cy, parent, 0, 0, 0);
    if (hwnd)
        SendMessageW(hwnd, WM_SETFONT, (WPARAM)getStdFont(4), 0);
    return hwnd;
}

static inline void makeButton(Button& button, HWND parent, LPCWSTR text, DWORD style, int x, int y, int cx,
                                int cy, int id)
{
    button.m_hWnd = CreateWindowExW(0, L"Button", text, style, x, y, cx, cy, parent, (HMENU)(INT_PTR)id, 0, 0);
    if (button.m_hWnd)
        button.setFontId(button.m_fontId);
    else
        GetLastError();
}

ThreeDxCtrlsPanel::ThreeDxCtrlsPanel(Popup* doc, glViewer* viewer)
    : CtrlsPanel(doc, viewer)
    , m_list(doc, viewer)
{
}

ThreeDxCtrlsPanel::~ThreeDxCtrlsPanel()
{
}

bool ThreeDxCtrlsPanel::onCreate()
{
    bool ok = CtrlsPanel::onCreate();
    if (!ok)
        return ok;
    int y = m_bottom;
    createLabel(m_hWnd, L"Texture:", 10, y, 150, 16);
    y += 16;
    makeButton(m_texCheck, m_hWnd, L"", 0x50000002, 15, y, 20, 20, 0x70);
    if (m_texCheck.m_hWnd)
        SetWindowSubclass(m_texCheck.m_hWnd, noEraseSubclassProc, 'chck', (DWORD_PTR)&m_texCheck);
    m_texture.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", L"", 0x50010080, 35, y, 130, 18, m_hWnd,
                                       (HMENU)0x71, 0, 0);
    if (m_texture.m_hWnd) {
        m_texture.setFontId(m_texture.m_fontId);
        SetWindowSubclass(m_texture.m_hWnd, tabSubclassProc, 'edit', (DWORD_PTR)&m_texture);
    }
    makeButton(m_browse, m_hWnd, L"...", WS_CHILD | WS_VISIBLE, 175, y - 1, 40, 20, 0x72);
    y += 22;
    makeButton(m_alpha, m_hWnd, L" Alpha Blending", 0x50010002, 15, y, 100, 18, 0x73);
    m_alpha.setCheck(g_preferences.getInt(L"3D View", L"Transparency", 0) == 1);
    int height = dword_5A0120 + 8 * dword_5A0124 + 4;
    y += 28;
    m_list.create(m_hWnd, (HMENU)0x65, 15, y, 210, height, 0x10800000, 0, 0);
    int by = y + height + 6;
    makeButton(m_show, m_hWnd, L"Show", WS_CHILD | WS_VISIBLE, 15, by, 50, 22, 0x6E);
    makeButton(m_hide, m_hWnd, L"Hide", WS_CHILD | WS_VISIBLE, 75, by, 50, 22, 0x6F);
    return true;
}

bool ThreeDxCtrlsPanel::onCommand(UINT code, HWND ctrl, UINT id)
{
    m_doc->setStatusText(0, 0);
    if (code)
        return CtrlsPanel::onCommand(code, ctrl, id);
    String path;
    bool result;
    if (id == 0x73) {
        bool enable = SendMessageW(ctrl, BM_GETCHECK, 0, 0) != BST_CHECKED;
        SendMessageW(ctrl, BM_SETCHECK, enable, 0);
        g_preferences.writeInt(L"3D View", L"Transparency", enable);
        glViewer* viewer = m_viewer;
        if (enable)
            viewer->m_drawFlags |= 1;
        else
            viewer->m_drawFlags &= ~1u;
        if (viewer->m_drawable) {
            viewer->m_drawable->setFlag(0xFF, false);
            viewer->m_drawable->setFlag(viewer->m_drawFlags, true);
        }
        result = true;
    } else if (id == 0x6E || id == 0x6F) {
        Model3D* model = dynamic_cast<Model3D*>(m_viewer->m_drawable);
        if (model) {
            int changed = 0;
            int i = 0;
            do {
                i = m_list.getNextSelected(i);
                if (i < 0)
                    break;
                Face* face = model->m_faces.get(i);
                if (face) {
                    if (id == 0x6E)
                        FIELD(BYTE, face, 0x24) &= ~2;
                    else if (id == 0x6F)
                        FIELD(BYTE, face, 0x24) |= 2;
                    ++changed;
                }
                ++i;
            } while (i >= 0);
            if (changed > 0) {
                static_cast<Doc*>(m_doc)->setModified(true);
                PostMessageW(m_hWnd, 0x430, (WPARAM)m_list.m_hWnd, 0);
            }
        }
        result = true;
    } else if (id == 0x70) {
        if (!m_texCheck.isChecked()) {
            {
                String text;
                path = *m_texture.getText(&text);
            }
            if (path.m_length) {
                Model3D* model = dynamic_cast<Model3D*>(m_viewer->m_drawable);
                ThreeDxView* view = dynamic_cast<ThreeDxView*>(m_doc);
                if (model && view) {
                    String dir(view->m_textureDir);
                    int pos = dir.indexOf(L"cloak");
                    if (pos != -1) {
                        String clothes = dir.left(pos + 6);
                        clothes << L"ClothTexture\\";
                        if (dir.containsNoCase(L"DeathEater"))
                            clothes << "DE";
                        else if (dir.containsNoCase(L"Elf"))
                            clothes << "EL";
                        else if (dir.containsNoCase(L"Human"))
                            clothes << "HU";
                        else if (dir.containsNoCase(L"Vile"))
                            clothes << "VI";
                        clothes << '\\';
                        dir = clothes;
                    }
                    model->m_textureName = dir + path;
                    model->releaseTexture();
                    if (!model->loadTexture(0)) {
                        String dds(dir);
                        dds.toParentPath();
                        dds << "DDS\\";
                        dds.append(path.m_buffer, path.m_length);
                        model->m_textureName = dds;
                        bool loaded = model->loadTexture(0);
                        if (!loaded) {
                            m_doc->setStatusText(L"The specified DDS file does not exist", 0);
                            return true;
                        }
                    }
                    m_viewer->m_renderMode = 1;
                    Drawable* drawable = m_viewer->m_drawable;
                    if (drawable && (!drawable->m_texture || drawable->m_texture == (GLuint)-1))
                        drawable->loadTexture(0);
                    m_viewer->clear();
                    m_texCheck.setCheck(true);
                    return true;
                }
            }
        }
        m_texCheck.setCheck(false);
        m_viewer->m_renderMode = 0;
        m_viewer->clear();
        result = true;
    } else if (id == 0x72) {
        String file(static_cast<Doc*>(m_doc)->getPath(), (unsigned)-1);
        file.removeFileName();
        if (file.startsWith(L"cloak")) {
            String race = file.substring(16, 2);
            race.toUpperAscii();
            file = L"cloak\\ClothTexture\\";
            file.append(race.m_buffer, race.m_length) << '\\';
        } else if (file.endsWith(L'\\')) {
            if (file.m_length > 5) {
                int removed = 0;
                while (file.m_length) {
                    ++removed;
                    --file.m_length;
                    file.m_buffer[file.m_length] = 0;
                    if (removed >= 4)
                        break;
                }
                if (file.endsWith(L'\\')) {
                    file << "DDS\\";
                    path = file;
                }
            }
        }
        if (FileDialogs_open(m_hWnd, &path, L"Texture Files", L"Choose texture file to be applied", false)) {
            m_texture.setText(path.m_buffer);
            Model3D* model = dynamic_cast<Model3D*>(m_viewer->m_drawable);
            if (model) {
                model->m_textureName = path;
                model->loadTexture(0);
            }
        }
        result = true;
    } else {
        result = CtrlsPanel::onCommand(0, ctrl, id);
    }
    return result;
}

void ThreeDxCtrlsPanel::onSize(WPARAM , int , int cy)
{
    HWND self = m_hWnd;
    RECT panel = {0};
    RECT list = {0};
    GetWindowRect(self, &panel);
    GetWindowRect(m_list.m_hWnd, &list);
    int y = cy - 34 + 6;
    SetWindowPos(m_list.m_hWnd, 0, 0, 0, list.right - list.left, cy - (list.top - panel.top) - 34,
                 SWP_NOMOVE | SWP_NOZORDER);
    SetWindowPos(m_show.m_hWnd, 0, 15, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    SetWindowPos(m_hide.m_hWnd, 0, 75, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

bool ThreeDxCtrlsPanel::ListFacet::onCreate()
{
    bool ok = ListCtrl::onCreate();
    if (!ok)
        return ok;
    addColumn(L"Indx", 40, 16, 0);
    addColumn(L"Pt 1", 35, 18, 1);
    addColumn(L"Pt 2", 35, 18, 2);
    addColumn(L"Pt 3", 35, 18, 3);
    addColumn(L"Vis.", 35, 18, 4);
    return true;
}

LRESULT ThreeDxCtrlsPanel::ListFacet::setItemCount(int count)
{
    LRESULT r = SendMessageW(m_hWnd, LVM_DELETEALLITEMS, 0, 0);
    for (int i = 0; i < count; ++i) {
        LVITEMW item;
        memset(&item.iItem, 0, 0x38);
        item.mask = LVIF_TEXT;
        if (i >= 0)
            item.iItem = i;
        else
            item.iItem = getCount(0);
        item.pszText = LPSTR_TEXTCALLBACKW;
        if (m_tileColumns.m_count >= m_tileLines) {
            item.mask |= LVIF_COLUMNS;
            item.cColumns = m_tileLines;
            item.puColumns = m_tileColumns.at(1);
        }
        r = SendMessageW(m_hWnd, LVM_INSERTITEMW, 0, (LPARAM)&item);
        WPARAM index = (WPARAM)r;
        if (r >= 0) {
            item.iItem = (int)r;
            item.mask = LVIF_TEXT;
            for (int column = 1; column < m_columnCount; ++column) {
                item.iSubItem = column;
                r = SendMessageW(m_hWnd, LVM_SETITEMTEXTW, index, (LPARAM)&item);
            }
        }
    }
    return r;
}

bool ThreeDxCtrlsPanel::ListFacet::onKey(short vk, int flags, bool down)
{
    if (!down)
        return false;
    if (vk == VK_RETURN || vk == VK_DELETE)
        return true;
    if (vk != VK_SPACE)
        return ListCtrl::onKey(vk, flags, down);
    int index = getNextSelected(0);
    if (index >= 0) {
        Model3D* model = dynamic_cast<Model3D*>(m_viewer->m_drawable);
        Face* face = model->m_faces.get(index);
        if (face) {
            BYTE f = FIELD(BYTE, face, 0x24);
            FIELD(BYTE, face, 0x24) = (f & 2) ? (BYTE)(f & 0xFD) : (BYTE)(f | 2);
            static_cast<Doc*>(m_doc)->setModified(true);
        }
    }
    return true;
}

HRESULT ThreeDxCtrlsPanel::ListFacet::onGetDispInfo(NMLVDISPINFOW* di)
{
    if (!m_faces)
        return E_FAIL;
    di->item.pszText = (LPWSTR)L"--";
    Face* face = m_faces->get(di->item.iItem);
    if (!face)
        return S_OK;
    int column = di->item.iSubItem;
    if (column == 4) {
        di->item.pszText = (LPWSTR)((FIELD(BYTE, face, 0x24) & 2) == 0 ? L"T" : L"F");
        return S_OK;
    }
    unsigned short value = 0;
    switch (column) {
    case 0: value = FIELD(unsigned short, face, 0x0C); break;
    case 1: value = FIELD(unsigned short, face, 0x0E); break;
    case 2: value = FIELD(unsigned short, face, 0x10); break;
    case 3: value = FIELD(unsigned short, face, 0x12); break;
    default: break;
    }
    swprintf_s(g_facetText, 16, L"%i", value);
    di->item.pszText = g_facetText;
    return S_OK;
}

void ThreeDxCtrlsPanel::ListFacet::onItemChanged(int index, UINT newState)
{
    if (!m_faces)
        return;
    Face* face = m_faces->get(index);
    if (!face)
        return;
    if (!newState)
        FIELD(BYTE, face, 0x24) &= ~1;
    else if (newState & 2)
        FIELD(BYTE, face, 0x24) |= 1;
}
