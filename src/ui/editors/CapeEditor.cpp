#include "CapeEditor.h"
#include "ctlhelpers_g4.h"
#include "_imports_p07.h"
#include "String.hpp"
#include "Bytes.h"
#include "iStream.h"
#include "StudioFrame.h"
#include "SDataView.h"
#include "Application.h"
#include <cwchar>
#include "AlertDial.h"

#define CAPE_TEXTURE(c) FIELD(int, c, 0x10)
#define CAPE_MODEL(c)   FIELD(int, c, 0x14)

static inline StudioFrame* g4_studio() { return g_studioFrame; }

bool CapeEditor::createEditor()
{
    HWND parent = m_owner->m_hWnd;
    m_icon.m_columns = 6;
    m_buttons = 1;
    return g4_createModal(this, parent, 370, 560, L"Cape Editor");
}

CapeEditor::~CapeEditor()
{
}

CapeEditor::CapeEditor(Window* owner, Data::Items* items, Data::Item* item)
    : ItemEditor(owner, items, item)
    , m_preview(Window_isClassicAppearance() ? GetSysColor(COLOR_BTNFACE) : 0xF0F0F0, 0)
{
    m_previewShown = false;
    SetCursor(hCursor);
    POINT pt;
    pt.x = 0;
    pt.y = 0;
    if (GetCursorPos(&pt))
        SetCursorPos(pt.x + 1, pt.y + 1);

    if (g4_studio()->isWorkspaceEmpty())
    {
        String path;
        g4_studio()->getWorkspacePath(&path);
        path << "cloak\\Character\\";
        switch (FIELD(unsigned char, m_item, 0x5A))
        {
        case 0: path << "Human\\1_humf.pc"; break;
        case 1: path << "Elf\\1_elmm.PC"; break;
        case 3: path << "DeathEater\\1_demf.pc"; break;
        case 4: path << "Vile\\1_vimm.pc"; break;
        default: break;
        }
        iStream in(false, static_cast<SDataView*>(owner)->getType());
        if (!studioLoadFile(&in, &path))
            static_cast<Streamable&>(m_capeModels).read(in, 0x66696C65 );
    }
    SetCursor(g_arrowCursor);
}

bool CapeEditor::onCommand(UINT code, HWND ctrl, UINT id)
{
    if (id == 103)
    {
        if (code == CBN_SELCHANGE)
        {
            showPreview((int)SendMessageW(m_model.m_hWnd, CB_GETCURSEL, 0, 0));
            return true;
        }
        return ItemEditor::onCommand(code, ctrl, id);
    }
    if (id != 200)
        return ItemEditor::onCommand(code, ctrl, id);
    if (m_previewButton.m_hWnd)
    {
        bool show = !m_previewShown;
        m_previewShown = show;
        if (show)
            m_previewShown = showPreview((int)SendMessageW(m_model.m_hWnd, CB_GETCURSEL, 0, 0));
        if (m_previewShown)
        {
            SetWindowPos(m_hWnd, 0, 0, 0, 652, 560, SWP_NOMOVE | SWP_NOZORDER);
            SetWindowTextW(m_previewButton.m_hWnd, L"<<");
            return true;
        }
        SetWindowPos(m_hWnd, 0, 0, 0, 370, 560, SWP_NOMOVE | SWP_NOZORDER);
        SetWindowTextW(m_previewButton.m_hWnd, L">>");
    }
    return true;
}

bool CapeEditor::onCreate()
{
    unsigned char type = FIELD(unsigned char, m_item, 0x54);
    bool light = type == 24;
    g4_setupIconPicker(this);
    createNameRow(0, 0, 210);

    int count = m_capeModels.m_capes.size();
    if (count <= 0)
    {
        g4_createLabel(L"Model:", 90, 48, 40, 18, m_hWnd, 4);
        m_modelEdit.createNumber(m_hWnd, (HMENU)190, 130, 45, 45, 18, FIELD(unsigned char, m_item, 0x56), 0, 0);
    }
    else
    {
        g4_createLabel(L"Model:", 90, 51, 40, 18, m_hWnd, 4);
        g4_createCombo(m_model, m_hWnd, 0x67, 130, 45, 160, 244);
        wchar_t buffer[128];
        for (int i = 0; i < count; ++i)
        {
            CapeData* cape = m_capeModels.m_capes.get(i);
            const wchar_t* texture = textureName(FIELD(char, m_item, 0x5A), CAPE_TEXTURE(cape));
            swprintf_s(buffer, 0x80, L"- %i -  (%s)", i, texture);
            g4_addComboString(m_model, buffer);
        }
        SendMessageW(m_model.m_hWnd, CB_SETCURSEL, FIELD(unsigned char, m_item, 0x56), 0);
        g4_createButton(m_previewButton, m_hWnd, 0xC8, L">>", 0x50000000, 310, 45, 30, 20);
    }

    m_preview.m_bitmapSize.cx = 256;
    m_preview.m_bitmapSize.cy = 512;
    RECT rc;
    SetRect(&rc, 375, 20, 631, 532);
    m_preview.create(m_hWnd, (HMENU)0xD2, &rc, WS_CHILD | WS_VISIBLE, 0, 0);

    int y = createDescriptionRow(0, 75, 320, 74);
    y = createEquipRow(0, y + 81, light, true, false);
    g4_createLabel(L"Characteristics:", 20, y, 150, 18, m_hWnd, 1);
    y += 20;
    g4_createLabel(L"Level:", 20, y + 3, 30, 18, m_hWnd, 4);
    m_level.createNumber(m_hWnd, (HMENU)122, 55, y, 45, 18, FIELD(short, m_item, 0x58), 0, 0);
    g4_createLabel(L"Def:", 120, y + 3, 30, 18, m_hWnd, 4);
    m_defense.createNumber(m_hWnd, (HMENU)129, 155, y, 45, 18, FIELD(unsigned short, m_item, 0x82), 0, 0);
    g4_createLabel(L"Resist:", 220, y + 3, 30, 18, m_hWnd, 4);
    m_resist.createNumber(m_hWnd, (HMENU)130, 255, y, 45, 18, FIELD(unsigned short, m_item, 0x84), 0, 0);

    int y2 = createStatsRows(20, y + 25, this);
    y2 = createHpSpMpRow(20, y2, this);
    y2 = createStatCountRow(20, y2 + 3, this, 0);
    g4_createLabel(L"Drop Group:", 20, y2 - 4, 35, 26, m_hWnd, 4);
    m_dropGroup.createNumber(m_hWnd, (HMENU)148, 55, y2, 45, 18, FIELD(short, m_item, 0xA4), 0, 0);
    g4_createLabel(L"Stack:", 120, y2 + 2, 30, 18, m_hWnd, 4);
    m_stack.createNumber(m_hWnd, (HMENU)149, 155, y2, 45, 18, FIELD(unsigned char, m_item, 0xA9), 0, 0);
    createColorRow(220, y2, 35, 80);
    createPriceRow(20, y2 + 28, this, 0);

    loadControls();
    return Modal::onCreate();
}

int CapeEditor::mapCommandToResult(int result)
{
    if (result != 1)
        return ItemEditor::mapCommandToResult(result);
    HWND combo = m_model.m_hWnd;
    if (!combo || (unsigned char)SendMessageW(combo, CB_GETCURSEL, 0, 0) != 0xFF)
        return ItemEditor::mapCommandToResult(result);
    AlertDial::alert(m_hWnd, L"You must select a valid model.", 0, 16);
    return -1;
}

void CapeEditor::saveControls()
{
    ItemEditor::saveControls();
    HWND combo = m_model.m_hWnd;
    LRESULT model;
    if (combo)
        model = SendMessageW(combo, CB_GETCURSEL, 0, 0);
    else
        model = m_modelEdit.getInt(FIELD(short, m_item, 0x58));
    FIELD(unsigned char, m_item, 0x56) = (unsigned char)model;
}

bool CapeEditor::showPreview(int index)
{
    if (index < 0)
        return false;
    if (index >= m_capeModels.m_capes.size())
        return false;
    CapeData* cape = m_capeModels.m_capes.get(index);
    if (!cape)
        return false;
    int modelIndex = CAPE_MODEL(cape);
    if (modelIndex < 0 || modelIndex >= m_capeModels.m_capeFiles.m_count)
        return false;
    String* modelFile = m_capeModels.m_capeFiles.m_items[modelIndex];
    const wchar_t* model = modelFile ? modelFile->m_buffer : 0;
    if (!model)
        return false;
    const wchar_t* texture = textureName(FIELD(char, m_item, 0x5A), CAPE_TEXTURE(cape));
    if (!texture)
        return false;

    const wchar_t* suffix = 0;
    const wchar_t* race = 0;
    switch (FIELD(unsigned char, m_item, 0x5A))
    {
    case 0: race = L"Human";      suffix = L"HU"; break;
    case 1: race = L"Elf";        suffix = L"EL"; break;
    case 3: race = L"DeathEater"; suffix = L"DE"; break;
    case 4: race = L"Vile";       suffix = L"VI"; break;
    default: break;
    }
    String modelPath;
    String texturePath;
    modelPath << L"cloak\\Character\\" << race << L"\\3DC\\" << model;
    texturePath << L"cloak\\ClothTexture\\" << suffix << '\\' << texture;
    bool ok;
    HBITMAP bitmap = (HBITMAP)ClientData::renderCape(modelPath.m_buffer, texturePath.m_buffer);
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

const wchar_t* CapeEditor::textureName(char faction, int index)
{
    CapeData* cape = m_capeModels.m_capes.get(index);
    if (!cape)
        return 0;
    int textureIndex = CAPE_TEXTURE(cape);
    const wchar_t* suffix;
    ClothTextures* textures;
    switch (faction)
    {
    case 0: suffix = L"HU"; textures = &m_texHU; break;
    case 1: suffix = L"EL"; textures = &m_texEL; break;
    case 3: suffix = L"DE"; textures = &m_texDE; break;
    case 4: suffix = L"VI"; textures = &m_texVI; break;
    default: return 0;
    }
    if (!textures)
        return 0;
    if (textures->isEmpty())
    {
        String path;
        g4_studio()->getWorkspacePath(&path);
        path << L"cloak\\ClothTexture\\CLOTH_TEXTHRE_" << suffix << L".CTL";
        textures->m_value = 1;
        Bytes data;
        if (g4_studio()->loadFromFile(&data, &path, false))
            return 0;
        static_cast<Streamable*>(textures)->read(data, 0x66696C65 );
    }
    if (textureIndex < 0)
        return 0;
    if (textureIndex >= textures->m_names.m_count)
        return 0;
    String* name = textures->m_names.m_items[textureIndex];
    if (!name)
        return 0;
    return name->m_buffer;
}
