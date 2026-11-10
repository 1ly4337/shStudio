#include "NpcEditor.h"
#include <commctrl.h>
#include <cwchar>
#include "StandardNPC.h"
#include "StudioFrame.h"
#include "ClientData.h"
#include "Model3D.h"
#include "MonsterModels.h"
#include "MetaModel3D.h"
#include "ptrArray.h"
#include "_imports_D.h"

NpcEditor::NpcEditor(StandardNPC* npc)
    : Modal(),
      m_npc(npc),
      m_title(),
      m_icon(),
      m_name(),
      m_welcome(),
      m_faction(),
      m_model(),
      m_expand(),
      m_preview(Window_isClassicAppearance() ? GetSysColor(COLOR_BTNFACE) : 0xF0F0F0, 0)
{
    m_width = 450;
    m_height = 350;
    m_expanded = false;
    m_tallWelcome = false;
}

NpcEditor::~NpcEditor()
{
}

bool NpcEditor::create(HWND parent)
{
    const wchar_t* title = m_title.c_str();
    m_buttons = 1;
    RECT rc;
    SetRect(&rc, CW_USEDEFAULT, CW_USEDEFAULT, m_width, m_height);
    bool ok = Window::create(parent, 0, &rc, 0x80C80000, 0x10101, title);
    if (ok)
    {
        m_owner = Modal_getTopLevelWindow(parent);
        Window_centerOn(m_hWnd, m_owner);
        show(true);
        return true;
    }
    return ok;
}

void NpcEditor::reserved73()
{
    m_icon.m_dropDown.show(false);
}

bool NpcEditor::onCreate()
{
    ClientData& clientData = g_studioFrame->m_client.m_clientData;
    ImageStrip* icons32 = clientData.getIcons(L"npc_face32");
    ImageStrip* icons64 = clientData.getIcons(L"npc_face64");
    if (icons32 && icons64)
    {
        m_icon.m_bitmaps = *icons64;
        m_icon.m_cells = *icons32;
    }
    m_icon.create(m_hWnd, (HMENU)101, 0, 20, 62, 60, 64, 0);

    int count = FIELD(int, &m_icon.m_bitmaps, 0x10) ;
    int index = 0;
    if (count <= 0)
    {
        index = -1;
    }
    else
    {
        int* ids = FIELD(int*, &m_icon.m_bitmaps, 0x08) ;
        while (*ids != m_npc->m_shape)
        {
            ++index;
            ++ids;
            if (index >= count)
            {
                index = -1;
                break;
            }
        }
    }
    m_icon.m_selected = index;
    m_icon.showIcon(index);
    FIELD(int, &m_icon.m_dropDown, 0x48) = index;

    HWND label = CreateWindowExW(0, L"Static", L"Name:", 0x50000000, 110, 25, 35, 18, m_hWnd, 0, 0, 0);
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(4), 0);
    m_name.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", L"", 0x50010080, 150, 23, 270, 18, m_hWnd,
                                    (HMENU)0x66, 0, 0);
    if (m_name.m_hWnd)
    {
        m_name.setFontId(m_name.m_fontId);
        SetWindowSubclass(m_name.m_hWnd, tabSubclassProc, 0x65646974 , (DWORD_PTR)&m_name);
    }
    SendMessageW(m_name.m_hWnd, EM_LIMITTEXT, 50, 0);

    label = CreateWindowExW(0, L"Static", L"Faction:", 0x50000000, 110, 53, 40, 18, m_hWnd, 0, 0, 0);
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(4), 0);
    m_faction.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"ComboBox", 0, (m_faction.m_dropLines & 3) | 0x50010000,
                                       150, 49, 60, 200, m_hWnd, (HMENU)0x67, 0, 0);
    if (m_faction.m_hWnd)
        m_faction.setFontId(m_faction.m_fontId);
    else
        GetLastError();
    if (SendMessageW(m_faction.m_hWnd, CB_ADDSTRING, 0, (LPARAM)L"Both") != -1)
        m_faction.m_items.StringTable::put((int)SendMessageW(m_faction.m_hWnd, CB_GETCOUNT, 0, 0), L"Both");
    if (SendMessageW(m_faction.m_hWnd, CB_ADDSTRING, 0, (LPARAM)L"Light") != -1)
        m_faction.m_items.StringTable::put((int)SendMessageW(m_faction.m_hWnd, CB_GETCOUNT, 0, 0), L"Light");
    if (SendMessageW(m_faction.m_hWnd, CB_ADDSTRING, 0, (LPARAM)L"Dark") != -1)
        m_faction.m_items.StringTable::put((int)SendMessageW(m_faction.m_hWnd, CB_GETCOUNT, 0, 0), L"Dark");
    SendMessageW(m_faction.m_hWnd, CB_SETCURSEL, m_npc->m_country, 0);

    label = CreateWindowExW(0, L"Static", L"Model's name: ", 0x50000002, 20, 90, 130, 18, m_hWnd, 0, 0, 0);
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(4), 0);
    m_model.m_hWnd = CreateWindowExW(0, L"ComboBoxEx32", 0, (m_model.m_dropLines & 3) | 0x50010000, 150, 85, 210, 350,
                                     m_hWnd, (HMENU)0x68, 0, 0);
    if (m_model.m_hWnd)
    {
        m_model.setFontId(m_model.m_fontId);
        SendMessageW(m_model.m_hWnd, CB_SETEXTENDEDUI, 1, 0);
        m_model.m_flags = 1;
    }
    else
    {
        GetLastError();
    }

    MonsterModels* models = clientData.getMonsterModels(L"npc");
    if (models)
    {
        int modelCount = models->size();
        for (int i = 0; i < modelCount; ++i)
        {
            MonsterModel* model = models->get(i);
            if (model)
                m_model.ComboBox::addItem(FIELD(const wchar_t*, model, 0x18), FIELD(int, model, 0x10));
        }
        SetWindowPos(m_model.m_hWnd, 0, 0, 0, 220, 350, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
        m_model.ComboBox::selectData(m_npc->m_shape);
    }
    else
    {
        SetWindowPos(m_model.m_hWnd, 0, 0, 0, 220, 350, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
        m_model.enable(false);
    }

    m_expand.m_hWnd = CreateWindowExW(0, L"Button", L">>", 0x50000000, 380, 85, 40, 22, m_hWnd, (HMENU)0x70, 0, 0);
    if (m_expand.m_hWnd)
        m_expand.setFontId(m_expand.m_fontId);
    else
        GetLastError();
    if (!models)
        m_expand.enable(false);

    label = CreateWindowExW(0, L"Static", L"Welcome Msg:", 0x50000000, 20, 115, 100, 16, m_hWnd, 0, 0, 0);
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(4), 0);
    int welcomeHeight = 54;
    if (m_tallWelcome)
        welcomeHeight = 108;
    m_welcome.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", L"", 0x50211044, 20, 130, 400, welcomeHeight, m_hWnd,
                                       (HMENU)0x69, 0, 0);
    if (m_welcome.m_hWnd)
    {
        m_welcome.setFontId(m_welcome.m_fontId);
        SetWindowSubclass(m_welcome.m_hWnd, tabSubclassProc, 0x65646974 , (DWORD_PTR)&m_welcome);
    }
    SendMessageW(m_welcome.m_hWnd, EM_LIMITTEXT, 0x200, 0);

    COLORREF face = Window_isClassicAppearance() ? GetSysColor(COLOR_BTNFACE) : 0xF0F0F0;
    m_preview.m_bkColor = face;
    m_preview.m_bitmapSize.cy = m_height;
    m_preview.m_bitmapSize.cx = 200;
    RECT rc;
    SetRect(&rc, m_width + 10, 0, m_width + 210, m_height);
    m_preview.create(m_hWnd, (HMENU)0x71, &rc, 0x50000000, 0, 0);

    createExtraControls();
    m_name.setText(m_npc->m_name.c_str());
    m_welcome.setText(m_npc->m_welcome.c_str());
    return Modal::onCreate();
}

bool NpcEditor::onCommand(UINT code, HWND ctrl, UINT id)
{
    if (id == 112)
    {
        m_expanded = !m_expanded;
        if (m_expanded)
        {
            LRESULT modelId;
            if (m_model.m_hWnd)
                modelId = SendMessageW(m_model.m_hWnd, CB_GETITEMDATA, SendMessageW(m_model.m_hWnd, CB_GETCURSEL, 0, 0), 0);
            else
                modelId = 0;
            m_expanded = showModel((int)modelId);
        }
        if (m_expanded)
        {
            SetWindowPos(m_hWnd, 0, 0, 0, m_width + 220, m_height, SWP_NOZORDER | SWP_NOMOVE);
            SetWindowTextW(m_expand.m_hWnd, L"<<");
        }
        else
        {
            SetWindowPos(m_hWnd, 0, 0, 0, m_width, m_height, SWP_NOZORDER | SWP_NOMOVE);
            SetWindowTextW(m_expand.m_hWnd, L">>");
        }
        return true;
    }
    if (id == 101)
    {
        if (code == 1)
        {
            int index = m_icon.m_selected;
            WPARAM modelId;
            if (index < 0 || index >= FIELD(int, &m_icon.m_bitmaps, 0x10) )
                modelId = 0;
            else
                modelId = FIELD(int*, &m_icon.m_bitmaps, 0x08) [index];
            m_model.ComboBox::selectData(modelId);
            if (m_expanded)
                showModel((int)modelId);
            return true;
        }
        return Modal::onCommand(code, ctrl, id);
    }
    if (id != 104 || code != 1)
        return Modal::onCommand(code, ctrl, id);
    LRESULT modelId = m_model.ComboBox::getSelectedData();
    m_icon.select((int)modelId);
    m_icon.invalidate(false);
    if (m_expanded)
        showModel((int)modelId);
    return true;
}

bool NpcEditor::showModel(int modelId)
{
    BmpCtrl* preview = &m_preview;
    if (preview->m_bitmap && preview->m_ownsBitmap)
        DeleteObject(preview->m_bitmap);
    preview->m_bitmap = 0;
    preview->m_bitmapSize.cx = 0;
    preview->m_bitmapSize.cy = 0;
    preview->invalidate(true);
    UpdateWindow(preview->m_hWnd);

    MonsterModels* models = g_studioFrame->m_client.m_clientData.getMonsterModels(L"npc");
    if (!models)
        return false;
    MonsterModel* model = models->get(modelId);
    if (!model)
        return false;

    String dataDir;
    String npcDir = *g_studioFrame->getWorkspacePath(&dataDir) + L"Npc\\";
    MetaModel3D meta;
    ptrArray<String*>& meshes = FIELD(ptrArray<String*>, model, 0x64);
    ptrArray<String*>& textures = FIELD(ptrArray<String*>, model, 0x90);
    int partCount = meshes.size();
    for (int i = 0; i < partCount; ++i)
    {
        Struct3D* part;
        {
            String ddsDir = npcDir + L"DDS\\";
            const wchar_t* textureName;
            if (i >= 0 && i < textures.m_count && textures.m_items[i])
                textureName = textures.m_items[i]->c_str();
            else
                textureName = 0;
            String texturePath = ddsDir + textureName;
            String meshDir = npcDir + L"3DC\\";
            const wchar_t* meshName;
            if (i >= 0 && i < meshes.m_count && meshes.m_items[i])
                meshName = meshes.m_items[i]->c_str();
            else
                meshName = 0;
            String meshPath = meshDir + meshName;
            part = ClientData::loadModel(meshPath, texturePath, 'k3DC');
        }
        if (part)
            meta.m_parts.add(part);
    }

    int height = m_height;
    COLORREF face = Window_isClassicAppearance() ? GetSysColor(COLOR_BTNFACE) : 0xF0F0F0;
    SIZE size;
    size.cx = 200;
    size.cy = height;
    bool ok;
    HBITMAP bitmap = meta.makeBitmap(face, size, 0, 'NPCs');
    if (bitmap)
    {
        m_preview.setBitmap(bitmap);
        ok = true;
    }
    else
    {
        ok = false;
    }
    return ok;
}

int NpcEditor::mapCommandToResult(int id)
{
    if (id != 1)
        return id;
    LRESULT modelId = -1;
    if (m_model.isEnabled())
    {
        if (!m_model.m_hWnd)
        {
            modelId = 0;
        }
        else
        {
            modelId = SendMessageW(m_model.m_hWnd, CB_GETITEMDATA, SendMessageW(m_model.m_hWnd, CB_GETCURSEL, 0, 0), 0);
            if (modelId == -1)
                return -1;
        }
    }
    if (!applyExtra())
        return -1;
    if (modelId != -1)
        m_npc->m_shape = (int)modelId;
    {
        String text;
        m_npc->m_name = *m_name.getText(&text);
    }
    {
        String text;
        m_npc->m_welcome = *m_welcome.getText(&text);
    }
    m_npc->m_country = (int)SendMessageW(m_faction.m_hWnd, CB_GETCURSEL, 0, 0);
    return 1;
}
