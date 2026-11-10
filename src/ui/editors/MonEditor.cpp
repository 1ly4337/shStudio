#include "MonEditor.h"
#include "MonsterModel.h"
#include "VertTexDialog.h"
#include "AlertDial.h"
#include "Window.h"
#include "mon_g1.h"
#include "_imports_p07.h"
#include <cwchar>
#include <cstdlib>

MonEditor::MonList3D::MonList3D(const String& folder, MonsterModel* model)
    : ListCtrl(),
      m_folder(folder),
      m_vertices(16),
      m_textures(16)
{
    m_vertices.m_allowDuplicates = true;
    m_textures.m_allowDuplicates = true;
    m_vertices = MM_VERTICES(model);
    m_textures = MM_TEXTURES(model);
    m_folder << L"3DC\\";
}

MonEditor::MonList3D::~MonList3D()
{
}

bool MonEditor::MonList3D::onCreate()
{
    if (!ListCtrl::onCreate())
        return false;
    addColumn(L"Index", 40, 16, 0);
    addColumn(L"Vertices", 130, 0, 1);
    addColumn(L"Textures", 130, 0, 2);
    addColumn(L"Format", 50, 2, 3);
    int count = m_vertices.size();
    for (int i = 0; i < count; ++i)
    {
        static wchar_t s_index[16];
        swprintf_s(s_index, 16, L"%li", i);
        insertTextItem(s_index, -1, -1);
        String* vert = (i >= 0 && i < m_vertices.m_count) ? m_vertices.m_items[i] : 0;
        setCellText(i, 1, vert ? vert->m_buffer : 0);
        String* tex = (i >= 0 && i < m_textures.m_count) ? m_textures.m_items[i] : 0;
        setCellText(i, 2, tex ? tex->m_buffer : 0);
        vert = (i >= 0 && i < m_vertices.m_count) ? m_vertices.m_items[i] : 0;
        String file = m_folder + (vert ? vert->m_buffer : 0);
        setCellText(i, 3, monModelVersion(file.m_buffer));
    }
    return true;
}

HRESULT MonEditor::MonList3D::onGetDispInfo(NMLVDISPINFOW* di)
{
    di->item.pszText = 0;
    if ((di->item.mask & LVIF_TEXT) == 0)
        return 0;
    di->item.pszText = (LPWSTR)L"";
    switch (di->item.iSubItem)
    {
    case 0:
    {
        static wchar_t s_text[16];
        swprintf_s(s_text, 16, L"%li", di->item.iItem);
        di->item.pszText = s_text;
        return 0;
    }
    case 1:
    {
        int index = di->item.iItem;
        if (index >= 0 && index < m_vertices.m_count && m_vertices.m_items[index])
        {
            di->item.pszText = m_vertices.m_items[index]->m_buffer;
            return 0;
        }
        di->item.pszText = 0;
        return 0;
    }
    case 2:
    {
        int index = di->item.iItem;
        if (index >= 0 && index < m_textures.m_count && m_textures.m_items[index])
        {
            di->item.pszText = m_textures.m_items[index]->m_buffer;
            return 0;
        }
        di->item.pszText = 0;
        return 0;
    }
    }
    return 0;
}

bool MonEditor::MonList3D::removeItem(int index)
{
    if (AlertDial::alert(m_hWnd, L"Delete this files-pair?", 0, MB_ICONQUESTION | MB_YESNO) != IDYES)
        return false;
    if (m_vertices.remove(index))
        m_vertices.m_ids.removeAt(index);
    if (m_textures.remove(index))
        m_textures.m_ids.removeAt(index);
    return true;
}

bool MonEditor::MonList3D::setItemString(int index, int column, const wchar_t* text)
{
    Strings* list;
    if (column == 1)
        list = &m_vertices;
    else if (column == 2)
        list = &m_textures;
    else
        return false;
    if (index >= 0 && index < list->size())
    {
        *list->at(index) = text;
        return true;
    }
    return false;
}

MonEditor::MonEditor(MonsterModel* model, const String& folder)
    : Modal(),
      m_model(model),
      m_folder(folder),
      m_list(folder, model)
{
}

MonEditor::~MonEditor()
{
}

bool MonEditor::create(HWND parent)
{
    m_buttons = 1;
    RECT rc;
    SetRect(&rc, CW_USEDEFAULT, CW_USEDEFAULT, 520, 580);
    if (!Window::create(parent, 0, &rc, 0x80C80000, 0x10101, L"MON Editor"))
        return false;
    m_owner = Modal_getTopLevelWindow(parent);
    Window_centerOn(m_hWnd, m_owner);
    show(true);
    return true;
}

static const wchar_t* monAnimationText(MonsterModel* model, long id)
{
    Strings& anims = MM_ANIMS(model);
    int count = anims.m_ids.m_count;
    for (int i = 0; i < count; ++i)
    {
        if (i >= 0 && i < count && anims.m_ids.m_items[i] == id)
        {
            String* text = anims.m_items[i];
            return text ? text->m_buffer : L"";
        }
    }
    return L"";
}

static void monCreateAnimation(MonEditor* editor, int slot, const wchar_t* label, int labelWidth, int labelX,
                               int editX, int y, int id)
{
    monCreateLabel(editor->m_hWnd, label, labelX, y + 3, labelWidth, 16, 4);
    monCreateEdit(editor->m_animations[slot], editor->m_hWnd, id, monAnimationText(editor->m_model, slot),
                  editX, y, 170, 18, tabSubclassProc);
}

bool MonEditor::onCreate()
{
    monCreateLabel(m_hWnd, L"Name:", 20, 22, 50, 16, 1);
    monCreateEdit(m_name, m_hWnd, 0x6E, L"", 70, 20, 410, 18, tabSubclassProc);
    m_name.setText(MM_NAME(m_model).m_buffer);

    int listHeight = dword_5A0120 + 4 + 6 * dword_5A0124;
    monCreateLabel(m_hWnd, L"3D files:", 20, 47, 50, 16, 1);
    m_list.create(m_hWnd, (HMENU)0x70, 70, 44, 410, listHeight, 0x10800000, 0, 0);

    int y = listHeight + 54;
    monCreateLabel(m_hWnd, L"Height:", 20, y + 3, 40, 16, 4);
    double height = MM_HEIGHT(m_model);
    m_height.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", L"", 0x50010080, 70, y, 60, 18, m_hWnd,
                                      (HMENU)0x78, 0, 0);
    if (m_height.m_hWnd)
    {
        m_height.setFontId(m_height.m_fontId);
        SetWindowSubclass(m_height.m_hWnd, decimalEditSubclassProc, 0x65646974 , (DWORD_PTR)&m_height);
        m_height.setFloat((float)height);
    }
    monCreateLabel(m_hWnd, L"Unkn:", 150, y + 3, 30, 16, 4);
    m_unknown.createNumber(m_hWnd, (HMENU)122, 190, y, 40, 18, MM_UNKN(m_model), 0, 0);
    monCreateButton(m_addFiles, m_hWnd, L"Add files", 0x72, 420, y - 4, 60, 22);

    int titleY = y + 27;
    monCreateLabel(m_hWnd, L"Animations:", 20, titleY, 100, 16, 1);
    int row = y + 44;
    int left = row;
    monCreateAnimation(this, 0, L"Walk:", 45, 20, 70, left, 0x82);       left += 22;
    monCreateAnimation(this, 1, L"Run:", 45, 20, 70, left, 0x83);        left += 22;
    monCreateAnimation(this, 2, L"Jump/Atk:", 50, 20, 70, left, 0x84);   left += 22;
    monCreateAnimation(this, 3, L"Attack 2:", 50, 20, 70, left, 0x85);   left += 22;
    monCreateAnimation(this, 4, L"Attack 3:", 50, 20, 70, left, 0x86);   left += 22;
    monCreateAnimation(this, 5, L"Die:", 45, 20, 70, left, 0x87);        left += 22;
    monCreateAnimation(this, 6, L"Breath:", 45, 20, 70, left, 0x88);     left += 22;
    monCreateAnimation(this, 7, L"Damage:", 45, 20, 70, left, 0x89);     left += 22;
    monCreateAnimation(this, 8, L"Idle:", 45, 20, 70, left, 0x8A);

    monCreateLabel(m_hWnd, L"Waves:", 260, titleY, 100, 16, 1);
    int right = row;
    monCreateAnimation(this, 9, L"Attack 1:", 50, 260, 310, right, 0x8C);  right += 22;
    monCreateAnimation(this, 10, L"Attack 2:", 50, 260, 310, right, 0x8D); right += 22;
    monCreateAnimation(this, 11, L"Attack 3:", 50, 260, 310, right, 0x8E); right += 22;
    monCreateAnimation(this, 12, L"Die:", 45, 260, 310, right, 0x8F);      right += 24;
    monCreateLabel(m_hWnd, L"Effects:", 260, right + 3, 100, 16, 1);
    right += 20;
    monCreateAnimation(this, 13, L"Attack 1:", 50, 260, 310, right, 0x96); right += 22;
    monCreateAnimation(this, 14, L"Attack 2:", 50, 260, 310, right, 0x97); right += 22;
    monCreateAnimation(this, 15, L"Attack 3:", 50, 260, 310, right, 0x98); right += 22;
    monCreateAnimation(this, 16, L"Die:", 45, 260, 310, right, 0x99);      right += 22;
    monCreateAnimation(this, 17, L"Attach:", 45, 260, 310, right, 0x9B);
    m_animations[17].enable(MM_FORMAT(m_model) == MON_FORMAT_MO4);
    return Modal::onCreate();
}

bool MonEditor::onCommand(UINT code, HWND ctrl, UINT id)
{
    if (code)
        return true;
    if (id == 0x72)
    {
        VertTexDialog dialog(m_folder.m_buffer);
        if (dialog.create(m_hWnd) && dialog.doModal() == 1)
        {
            m_list.m_vertices.add(dialog.m_vertFile.m_buffer);
            m_list.m_textures.add(dialog.m_texFile.m_buffer);
            int count = m_list.getCount(0);
            static wchar_t s_index[16];
            swprintf_s(s_index, 16, L"%li", count);
            int row = m_list.insertTextItem(s_index, -1, -1);
            m_list.setCellText(row, 1, dialog.m_vertFile.m_buffer);
            m_list.setCellText(row, 2, dialog.m_texFile.m_buffer);
            String file = (m_folder + L"3DC\\") + dialog.m_vertFile;
            m_list.setCellText(row, 3, monModelVersion(file.m_buffer));
        }
    }
    else if (id <= 11)
    {
        m_result = mapCommandToResult(id);
        if (m_result == 1 && m_modelessOwner)
            SendMessageW(m_modelessOwner->m_hWnd, 0x403, 1, 0);
        if (m_result != -1)
            PostMessageW(m_hWnd, WM_CLOSE, 0, 0);
    }
    return true;
}

int MonEditor::mapCommandToResult(int id)
{
    if (id != 1)
        return id;
    {
        String text;
        MM_NAME(m_model) = *m_name.getText(&text);
    }
    MM_VERTICES(m_model) = m_list.m_vertices;
    MM_TEXTURES(m_model) = m_list.m_textures;
    float height;
    {
        String text;
        m_height.getText(&text);
        if (text.m_length)
            height = (float)_wtof(text.m_buffer);
        else
            height = 0.0f;
    }
    MM_HEIGHT(m_model) = height;
    MM_UNKN(m_model) = (BYTE)m_unknown.getInt(0);
    Strings& anims = MM_ANIMS(m_model);
    anims.deleteAll();
    anims.m_ids.m_count = 0;
    for (int i = 0; i < 18; ++i)
    {
        String text;
        m_animations[i].getText(&text);
        if (text.m_length)
            anims.add(i, text.m_buffer);
    }
    return 1;
}
