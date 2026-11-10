#include "MltView.h"
#include "ModelsMLT.h"
#include "ModelMLT.h"
#include "MltEditor.h"
#include "Model3D.h"
#include "StdTextView.h"
#include "StudioFrame.h"
#include "ClientNode.h"
#include "ClientData.h"
#include "Data_Items.h"
#include "Data_Item.h"
#include "AlertDial.h"
#include "String.hpp"
#include <cwchar>
#include "_imports_p06.h"

MltView::MltView(FileTree* file, ModelsMLT* models)
    : ModelView(file, models, &m_list)
    , m_list(this)
{
    if (models)
    {
        m_models = models;
    }
    else
    {
        m_models = new ModelsMLT();
        m_content = m_models;
        FIELD(Serializable*, this, 0x50) = m_models;
    }
    m_list.m_models = m_models;
}

MltView::~MltView()
{
}

bool MltView::onInitMenu(HMENU menu)
{
    setStatusText(0, 0);
    HWND list = GetDlgItem(m_hWnd, 0x65);
    if (list)
        SendMessageW(list, WM_INITMENU, (WPARAM)menu, 0);
    enableMenuItem(0xE124, IsClipboardFormatAvailable(0x206) == 1);
    return true;
}

LPCWSTR MltView::getCommandHelp(UINT id)
{
    switch (id)
    {
    case 0xE412:
        return L"Remove ev_10wc_xxx model";
    case 0xE430:
        return L"Import text fields from a language file";
    case 0xE431:
        return L"Import definition from database";
    }
    if (id != 0xE435)
        return 0;
    return L"Change codepage...";
}

void MltView::fillScriptMenu(HMENU scripts)
{
    appendMenuItem(scripts, 0xE412);
    appendMenuItem(scripts, 0xE411);
    appendMenuItem(scripts, 0xE435);
    appendMenuItem(scripts, 0xE430);
}

int MltView::getMenuId()
{
    return 0xAA;
}

bool MltView::onCreate()
{
    if (!ModelView::onCreate())
        return false;
    HWND parent = m_hWnd;
    RECT rcBuf;
    RECT* rc = getClientRect(&rcBuf);
    int height = rc->bottom - rc->top;
    if (!m_list.create(parent, (HMENU)0x65, rc->left, rc->top, rc->right - rc->left, height,
                       0x50000000, 0, height))
        return false;
    m_list.setItemCount(m_models->size());
    return true;
}

void MltView::selectModel(int index)
{
    m_selected = index;
    if (!m_view3D.isVisible())
        return;
    m_view3D.clearModel();
    ModelMLT* model = m_models->get(index);
    if (!model)
        return;
    String dir(FIELD(String, m_data, 0x10));
    dir.removeFileName();
    String vertex(dir);
    vertex << L"3DC\\" << model->m_vertex;
    String texture(dir);
    texture << L"DDS\\" << model->m_texture;
    Model3D* model3d = new Model3D('k3DC');
    if (model3d->loadFiles(vertex.c_str(), texture.c_str()))
    {
        if (model3d)
            delete model3d;
    }
    else
    {
        m_view3D.setModel(model3d);
        model->m_vrs3 = static_cast<Serializable*>(model3d)->serialV5();
        m_list.updateItem(index);
    }
}

bool MltView::onCommand(UINT code, HWND ctrl, UINT id)
{
    if ((code | 1) != 1 || ctrl)
        return ModelView::onCommand(code, ctrl, id);
    if (id == 0xE411)
    {
        String text;
        static_cast<Streamable*>(m_models)->toText(text, 'dump');
        StdTextView* view = new StdTextView();
        if (view->openWindow())
            view->setContent(text);
        else
            delete view;
        return true;
    }
    if (id == 0xE412)
    {
        removeEv10wcModel();
        return true;
    }
    return ModelView::onCommand(code, ctrl, id);
}

void MltView::editModel(int index)
{
    ModelMLT* model = m_models->get(index);
    if (!model)
        return;
    ModelMLT copy(*model, 0);
    String dir(FIELD(String, m_data, 0x10));
    dir.removeFileName();
    MltEditor editor(&copy, dir.c_str());
    if (editor.create(m_hWnd) && editor.doModal() == 1)
    {
        static_cast<Streamable*>(model)->copyFrom(static_cast<Streamable*>(&copy));
        setModified(true);
        selectModel(index);
    }
}

void MltView::removeEv10wcModel()
{
    FileTree* file = m_data;
    const wchar_t* name = FIELD(String, file, 0x30).c_str();
    const String& path = FIELD(String, file, 0x10);
    if (!name || path.length() < 9
        || _wcsnicmp(path.c_str() + path.length() - 9, L"upper.MLT", 9) != 0)
    {
        AlertDial::alert(m_hWnd, L"This script applies on xxx_upper.mlt files only", 0, 0x10);
        return;
    }
    int count = m_models->size();
    int i = 0;
    for (; i < count; ++i)
    {
        ModelMLT* model = m_models->get(i);
        if (model->m_texture.length() >= 8 && wcsncmp(model->m_texture.c_str(), L"ev_10wc_", 8) == 0)
            break;
    }
    if (i >= count)
    {
        AlertDial::alert(m_hWnd, L"No record contains the name 'ev_10wc_xxx'", 0, 0x10);
        return;
    }
    ModelMLT* removed = m_models->detach(i);
    if (!removed)
        return;
    setModified(true);
    m_list.invalidate(true);
    int removedId = removed->m_id;
    delete removed;
    if (removedId < 0)
    {
        AlertDial::alert(m_hWnd, L"No record contains the name 'ev_10wc_xxx'", 0, 0x10);
        return;
    }
    if (name[2] != L'm')
        return;
    FileTree* itemFile = g_studioFrame->findFile(L"Item\\Item.sdata");
    if (!itemFile)
    {
        String msg;
        msg << L"The item.sdata file was not found." << L"\r\n"
            << L"The record is deleted but the upper gears are not updated.";
        AlertDial::alert(m_hWnd, msg.c_str(), 0, 0x30);
        return;
    }
    StudioFrame* frame = g_studioFrame;
    ClientData& client = frame->m_client.m_clientData;
    client.loadSData(&client.m_items, L"Item\\item.sdata", L"BinarySData\\DBItemData.sdata", 0);
    String race;
    race.append(name, 2);
    int types[3] = { 17, 67, 73 };
    if (race == L"de" || race == L"vi")
    {
        types[0] = 32;
        types[1] = 82;
        types[2] = 82;
    }
    race << name[3];
    int classB = -2;
    int classA;
    if (race == L"huf")
    {
        classA = 0;
        classB = 1;
    }
    else if (race == L"hum")
        classA = 5;
    else if (race == L"elm")
        classA = 4;
    else if (race == L"elr")
    {
        classA = 3;
        classB = 2;
    }
    else if (race == L"def")
    {
        classA = 0;
        classB = 1;
    }
    else if (race == L"der")
        classA = 3;
    else if (race == L"vim")
    {
        classA = 4;
        classB = 5;
    }
    else
        classA = race == L"vir" ? 2 : -2;
    for (int t = 0; t < 3; ++t)
    {
        for (int typeId = 0; typeId < 256; ++typeId)
        {
            Data::Items& items = client.m_items;
            int n = items.m_count;
            int k = 0;
            for (; k < n; ++k)
            {
                Data::Item* item = items.m_items[k];
                if (item->m_type == types[t] && item->m_typeId == typeId)
                    break;
            }
            if (k >= n)
                continue;
            Data::Item* item = items.m_items[k];
            BYTE modelIndex = item->m_model;
            if ((int)modelIndex > removedId && item->m_class[classA] == 1
                && (classB == -2 || item->m_class[classB] == 1))
            {
                BYTE icon = item->m_icon;
                item->m_model = modelIndex - 1;
                if (icon > 0x16)
                    item->m_icon = icon - 1;
            }
        }
    }
    FIELD(int, itemFile, 0xFC) = 2;
    String msg;
    msg << L"The record ev_10wc_xxx is deleted from the model list." << L"\r\n"
        << L"The item.sdata is updated to assign correct model indexes.";
    AlertDial::alert(m_hWnd, msg.c_str(), 0, 0x40);
}
