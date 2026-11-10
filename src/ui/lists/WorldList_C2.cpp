#include "WorldList.h"
#include "WorldView.h"
#include "WorldDropManager.h"
#include "World.h"
#include "kWorld_Items.h"
#include "kWorld_Element.h"
#include "kWorld_Sounds.h"
#include "kWorld_Portal.h"
#include "kWorld_Spawn.h"
#include "kWorld_NamedArea.h"
#include "kWorld_NPC.h"
#include "kWorld_Vertice.h"
#include "MapHeight.h"
#include "MapHeightView.h"
#include "CoordsEditor.h"
#include "CoordsEditor2.h"
#include "VerticeEditor.h"
#include "WldPortalEditor.h"
#include "SpawnAreaEditor.h"
#include "NamedAreaEditor.h"
#include "WldNpcEditor.h"
#include "ZoneAreaEditor.h"
#include "SpotAreaEditor.h"
#include "SuperModule.h"
#include "Name.h"
#include "Node.h"
#include "Cell.h"
#include "String.hpp"
#include "StringTable.h"
#include "fileutil.h"
#include "Bytes.h"
#include "ImageList.h"
#include "AlertDial.h"
#include "FileDialogs.h"
#include "StudioFrame.h"
#include "world_common.h"
#include <commctrl.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <cwchar>

#define WL_VIEW  (static_cast<WorldView*>(m_view))
#define WL_WORLD m_world
#define WL_TREE  m_tree.m_hWnd

bool WorldList::onCreate()
{
    if (!TreeList::onCreate())
        return false;

    {
        ImageList images;
        images.m_cx = 16;
        images.load(0xA2, 0x20);
        HIMAGELIST himl = images.m_hList;
        images.m_hList = 0;
        SendMessageW(WL_TREE, TVM_SETIMAGELIST, TVSIL_NORMAL, (LPARAM)himl);
        SendMessageW(WL_TREE, TVM_SETIMAGELIST, TVSIL_STATE, (LPARAM)himl);

        LVCOLUMNW column;
        memset(&column, 0, sizeof(column));
        column.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
        column.fmt = 0;
        column.cx = 130;
        column.pszText = (LPWSTR)L"Item Name";
        insertColumn(&column, 'name');

        struct ColumnDef { int width; LPCWSTR text; int id; };
        static const ColumnDef defs[] = {
            { 60, L"X", 'posX' }, { 60, L"Y", 'posY' }, { 60, L"Z", 'posZ' },
            { 55, L"r", 'rotX' }, { 55, L"s", 'rotY' }, { 55, L"t", 'rotZ' },
            { 55, L"u", 'x3__' }, { 55, L"v", 'y3__' }, { 55, L"w", 'z3__' },
            { 55, L"k", 'unkn' },
        };
        for (int i = 0; i < 10; ++i)
        {
            int fmt = 0xFFFF;
            memset(&column, 0, sizeof(column));
            if (!m_columnCount)
                fmt = 0xFF0;
            column.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
            column.fmt = fmt;
            column.cx = defs[i].width;
            column.pszText = (LPWSTR)defs[i].text;
            insertColumn(&column, defs[i].id);
        }

        m_dropTarget = new WorldDropManager(this, WL_WORLD);
    }
    return true;
}

void WorldList::onSelChanged(Node* oldNode, Node* node)
{
    WorldView* view = WL_VIEW;
    view->setStatusText(0, 0);
    bool selected = node != 0;
    view->enableMenuItem(0xE123, selected);
    view->enableMenuItem(0xE122, selected);
    view->enableMenuItem(0xE12B, selected);
    view->enableMenuItem(0xE12C, selected);

    WorldView* preview = WL_VIEW;
    if (dynamic_cast<kWorld::Element*>(node))
        view->enableMenuItem(0xE12A, true);

    Name* name = dynamic_cast<Name*>(node);
    if (!name)
    {
        preview->vslot102(0);
        return;
    }

    String file(name->m_file);
    if (file.m_length >= 5 && !_wcsnicmp(file.m_buffer + file.m_length - 5, L".smod", 5))
    {
        String path;
        C2_STUDIO->getWorkspacePath(&path);
        LPARAM parent = SendMessageW(WL_TREE, TVM_GETNEXTITEM, TVGN_PARENT, node->m_data);
        {
            String folder = getItemText((HTREEITEM)parent);
            ((path << L"Entity\\").append(folder.m_buffer, folder.m_length) << L'\\')
                .append(file.m_buffer, file.m_length);
        }

        Bytes bytes(0);
        if (!studioLoadFile(&bytes, &path))
        {
            SuperModule* module = new SuperModule();
            static_cast<Serializable*>(module)->read(bytes, 'file');
            preview->vslot102(module);
        }
    }
}

static bool WorldList_openWorkspaceItem(String* path)
{
    FileTree* item = (FileTree*)C2_STUDIO->findFile(path->m_buffer);
    if (!item)
        return false;
    C2_STUDIO->reopenDocument(item);
    return true;
}

bool WorldList::onItemActivate(HTREEITEM item, WPARAM keys)
{
    String path;

    Node* node = getItemNode(item);
    if (!node)
    {
        String text = getItemText(item);
        if (text.m_length >= 4 && !_wcsnicmp(text.m_buffer + text.m_length - 4, L".wtr", 4))
        {
            (path << L"Entity\\Water\\").append(text.m_buffer, text.m_length);
            FileTree* found = (FileTree*)C2_STUDIO->findFile(path.m_buffer);
            if (found)
                C2_STUDIO->reopenDocument(found);
        }
        else if (text.endsWithNoCase(L".dds") || text.endsWithNoCase(L".tga"))
        {
            (path << L"Terrain\\Detail\\").append(text.m_buffer, text.m_length);
            FileTree* found = (FileTree*)C2_STUDIO->findFile(path.m_buffer);
            if (found)
            {
                C2_STUDIO->reopenDocument(found);
            }
            else if (text.endsWithNoCase(L".tga"))
            {
                path.removeExtension();
                path << L".dds";
                WorldList_openWorkspaceItem(&path);
            }
        }
        else
        {
            kWorld::Items* world = WL_WORLD->getItems();
            if (text.equalsNoCase(world->m_inner))
            {
                (path << L"World\\Dungeon\\").append(text.m_buffer, text.m_length);
                WorldList_openWorkspaceItem(&path);
            }
        }
        return true;
    }

    if (MapHeight* heights = dynamic_cast<MapHeight*>(node))
    {
        MapHeightView* view = new MapHeightView(heights);
        if (view && !view->open())
            delete view;
        return true;
    }

    if (Name* name = dynamic_cast<Name*>(node))
    {
        String& file = name->m_file;
        if (file.endsWithNoCase(L".smod"))
        {
            String itemText = getItemText(item);
            LPARAM parent = SendMessageW(WL_TREE, TVM_GETNEXTITEM, TVGN_PARENT, (LPARAM)item);
            {
                String folder = getItemText((HTREEITEM)parent);
                ((path << L"Entity\\").append(folder.m_buffer, folder.m_length) << '\\')
                    .append(itemText.m_buffer, itemText.m_length);
            }
            WorldList_openWorkspaceItem(&path);
            return true;
        }
        if (file.endsWithNoCase(L".vani"))
        {
            String itemText = getItemText(item);
            (path << L"Entity\\VAni\\").append(itemText.m_buffer, itemText.m_length);
            WorldList_openWorkspaceItem(&path);
            return true;
        }
        if (file.endsWithNoCase(L".dg"))
        {
            String itemText = getItemText(item);
            (path << L"World\\dungeon\\").append(itemText.m_buffer, itemText.m_length);
            WorldList_openWorkspaceItem(&path);
            return true;
        }
        if (file.endsWithNoCase(L".eft"))
        {
            (path << L"Effect\\").append(file.m_buffer, file.m_length);
            WorldList_openWorkspaceItem(&path);
            return true;
        }
        if (!file.endsWithNoCase(L".wav"))
            return true;

        {
            String root;
            C2_STUDIO->getWorkspacePath(&root);
            path = root;
        }
        if (!path.m_length)
            return false;
        path << L"Sound\\";
        if (fileExistsIn(path.m_buffer, file.m_buffer))
        {
            path.append(file.m_buffer, file.m_length);
        }
        else
        {
            bool inBack;
            {
                String back = path + L"back\\";
                inBack = fileExistsIn(back.m_buffer, file.m_buffer);
            }
            String* s;
            if (inBack)
            {
                s = &(path << "back\\");
            }
            else
            {
                bool inMusic;
                {
                    String music = path + L"Music\\";
                    inMusic = fileExistsIn(music.m_buffer, file.m_buffer);
                }
                if (!inMusic)
                    return false;
                s = &(path << L"Music\\");
            }
            s->append(file.m_buffer, file.m_length);
        }

        String args;
        args << L"/play /close ";
        ((args << '"').append(path.m_buffer, path.m_length)) << '"';
        if ((int)(INT_PTR)ShellExecuteW(m_hWnd, L"open", L"sndrec32.exe", args.m_buffer, 0, SW_SHOWNORMAL) < 32)
            ShellExecuteW(0, L"open", path.m_buffer, 0, 0, SW_SHOWNORMAL);
        return true;
    }

    kWorld::Element::Coord*   coord   = dynamic_cast<kWorld::Element::Coord*>(node);
    kWorld::Portal*           portal  = dynamic_cast<kWorld::Portal*>(node);
    kWorld::Vertice*          vertice = dynamic_cast<kWorld::Vertice*>(node);
    kWorld::Spawn*            spawn   = dynamic_cast<kWorld::Spawn*>(node);
    kWorld::NamedArea*        area    = dynamic_cast<kWorld::NamedArea*>(node);
    kWorld::NPC*              npc     = dynamic_cast<kWorld::NPC*>(node);
    kWorld::Sounds::ZoneArea* zone    = dynamic_cast<kWorld::Sounds::ZoneArea*>(node);
    kWorld::Sounds::SpotArea* spot    = dynamic_cast<kWorld::Sounds::SpotArea*>(node);

    WorldView* view = WL_VIEW;
    WorldEditor* editor;
    if (coord)
    {
        if (coord->m_mode == 1)
            editor = new CoordsEditor(view, coord);
        else
            editor = new CoordsEditor2(view, coord);
    }
    else if (portal)
        editor = new WldPortalEditor(view, portal);
    else if (vertice)
        editor = new VerticeEditor(view, vertice);
    else if (spawn)
        editor = new SpawnAreaEditor(view, spawn);
    else if (area)
        editor = new NamedAreaEditor(view, area);
    else if (npc)
        editor = new WldNpcEditor(view, npc);
    else if (zone)
        editor = new ZoneAreaEditor(view, zone);
    else if (spot)
        editor = new SpotAreaEditor(view, spot);
    else
        return TreeList::onItemActivate(item, keys);

    if (!editor)
        return TreeList::onItemActivate(item, keys);

    if (editor->create(m_hWnd) && editor->doModal() == 1)
    {
        onItemsChanged();
        invalidateItem((int)(INT_PTR)item);
        SetFocus(WL_TREE);
    }
    delete editor;
    return true;
}

bool WorldList::onDropFile(LPCWSTR path, HWND target, int targetId)
{
    if (target != WL_TREE)
        return false;
    if (WL_WORLD->isEmpty())
        return false;

    String ext;
    ext.append(PathFindExtensionW(path), (unsigned)-1);
    ext.toLowerInPlace();
    if (ext.String::sub_4121F0(L".smod") && ext.String::sub_4121F0(L".mani") && ext.String::sub_4121F0(L".vani") &&
        ext.String::sub_4121F0(L".wav"))
        return false;

    String file;
    file.append(path, (unsigned)-1);
    String entityRoot;
    C2_STUDIO->getWorkspacePath(&entityRoot);
    entityRoot << L"Entity\\";

    if (PathIsRelativeW(entityRoot.m_buffer))
    {
        AlertDial::alert(m_hWnd, L"Dropping files into an archive is not supported.\n"
                                 L"Drop files into archive and then from archive to world.", 0, MB_ICONERROR);
        return false;
    }
    if (!file.startsWith(entityRoot))
    {
        String message;
        (message << L"The dropped files must belong to the Entity folder of your data-set\n")
                .append(entityRoot.m_buffer, entityRoot.m_length)
            << L".  Move the files and try again.";
        AlertDial::alert(m_hWnd, message.m_buffer, 0, MB_ICONERROR);
        return false;
    }

    file = file.right(file.m_length - entityRoot.m_length);
    int slash = file.lastIndexOf(L'\\');
    if (slash < 0)
        return false;

    String folder = file.left(slash);
    file = file.right(file.m_length - folder.m_length - 1);
    folder.toLowerInPlace();

    kWorld::Items* world = WL_WORLD->getItems();
    kWorld::Section* section;
    if (folder == L"building")
        section = &world->m_building;
    else if (folder == L"grass")
        section = &world->m_grass;
    else if (folder == L"mani")
        section = &world->m_mani;
    else if (folder == L"object")
        section = &world->m_object;
    else if (folder == L"shape")
        section = &world->m_shape;
    else if (folder == L"tree")
        section = &world->m_tree;
    else if (folder == L"vani")
        section = &world->m_vani2;
    else
        section = 0;

    const wchar_t* name = file.m_buffer;
    if (!section)
    {
        String message;
        ((message << L"The path \"").append(folder.m_buffer, folder.m_length))
            << L"\" is not valid.\nOnly direct subfolders of \"Entity\" are allowed.";
        AlertDial::alert(m_hWnd, message.m_buffer, 0, MB_ICONERROR);
    }
    else if (section->hasItem(name))
    {
        String message;
        ((((message << L"The file \"").append(name, file.m_length)) << L"\" is already present in the block \"")
             .append(folder.m_buffer, folder.m_length))
            << L"\".";
        AlertDial::alert(m_hWnd, message.m_buffer, 0, MB_ICONERROR);
    }
    else
    {
        Node* added = section->addItem(name);
        HTREEITEM item = insertNode((HTREEITEM)(INT_PTR)section->m_data, added, 3, 0, TVI_LAST);
        SendMessageW(WL_TREE, TVM_ENSUREVISIBLE, 0, (LPARAM)item);
        SendMessageW(WL_TREE, TVM_SELECTITEM, TVGN_CARET, (LPARAM)item);
    }
    return false;
}

void WorldList::populate(LPCWSTR rootText)
{
    SetCursor(LoadCursorW(0, IDC_WAIT));
    deleteAllItems();

    TVINSERTSTRUCTW tvis;
    memset(&tvis, 0, sizeof(tvis));
    tvis.item.pszText = (LPWSTR)rootText;
    tvis.hParent = TVI_ROOT;
    tvis.hInsertAfter = TVI_ROOT;
    tvis.item.iImage = 0;
    tvis.item.iSelectedImage = 0;
    tvis.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN;
    tvis.item.cChildren = 100;
    LPARAM root = SendMessageW(WL_TREE, TVM_INSERTITEMW, 0, (LPARAM)&tvis);
    m_64 = (int)root;
    if (root)
    {
        World* world = WL_WORLD;
        if (world->m_dungeon)
        {
            insertWorld(world->m_dungeon);
        }
        else
        {
            World::Field* field = world->m_field;
            if (field)
            {
                Node* heights = &field->m_mapHeight;
                LPARAM heightsItem;
                if (!heights)
                    heightsItem = 0;
                else
                    heightsItem = (LPARAM)insertCell(new Cell(heights), (HTREEITEM)root, 12, 0, TVI_LAST);

                World::Field::TextSounds& textures = WL_WORLD->m_field->m_textSounds;
                int count = textures.size();
                for (int i = 0; i < count; ++i)
                {
                    LPWSTR text = textures.get(i)->m_texture.m_buffer;
                    HTREEITEM after = TVI_LAST;
                    if (heightsItem == (LPARAM)TVI_ROOT)
                        after = TVI_ROOT;
                    memset(&tvis, 0, sizeof(tvis));
                    tvis.hParent = (HTREEITEM)heightsItem;
                    tvis.hInsertAfter = after;
                    tvis.item.pszText = text;
                    tvis.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
                    tvis.item.iImage = 9;
                    tvis.item.iSelectedImage = 9;
                    SendMessageW(WL_TREE, TVM_INSERTITEMW, 0, (LPARAM)&tvis);
                }
                insertWorld(WL_WORLD->m_field);
            }
        }
        SendMessageW(WL_TREE, TVM_EXPAND, TVE_EXPAND, (LPARAM)m_64);
    }
    SetCursor(LoadCursorW(0, IDC_ARROW));
}

void WorldList::insertWorld(kWorld::Items* world)
{
    if (world->m_inner.m_length)
    {
        LPWSTR caption = world->m_inner.m_buffer;
        LPARAM root = (LPARAM)m_64;
        HTREEITEM after = TVI_LAST;
        if (root == (LPARAM)TVI_ROOT)
            after = TVI_ROOT;
        TVINSERTSTRUCTW tvis;
        memset(&tvis, 0, sizeof(tvis));
        tvis.item.pszText = caption;
        tvis.hParent = (HTREEITEM)root;
        tvis.hInsertAfter = after;
        tvis.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
        tvis.item.iImage = 1;
        tvis.item.iSelectedImage = 1;
        SendMessageW(WL_TREE, TVM_INSERTITEMW, 0, (LPARAM)&tvis);
    }

    insertItems((LPARAM)m_64, &world->m_dungeonSection);
    insertItems((LPARAM)m_64, &world->m_building);
    insertItems((LPARAM)m_64, &world->m_shape);
    insertItems((LPARAM)m_64, &world->m_tree);
    insertItems((LPARAM)m_64, &world->m_grass);
    insertItems((LPARAM)m_64, &world->m_vani);
    insertItems((LPARAM)m_64, &world->m_vani2);
    insertItems((LPARAM)m_64, &world->m_mani);
    insertItems((LPARAM)m_64, &world->m_object);

    {
        int count = world->m_vertices.size();
        LPARAM parent = (LPARAM)m_64;
        Node* node = &world->m_effects;
        HTREEITEM group = node ? insertCell(new Cell(node), (HTREEITEM)parent, 5, count, TVI_LAST) : 0;
        for (int i = 0; i < count; ++i)
        {
            Node* child = world->m_vertices.m_items[i];
            if (child)
                insertCell(new Cell(child), group, 4, 0, TVI_LAST);
        }
    }
    {
        int count = world->m_portals.size();
        Node* node = static_cast<ToggleNode*>(&world->m_portals);
        HTREEITEM group = node ? insertCell(new Cell(node), (HTREEITEM)m_64, 5, count, TVI_LAST) : 0;
        for (int i = 0; i < count; ++i)
            if (Node* child = world->m_portals.m_items[i])
                insertCell(new Cell(child), group, 4, 0, TVI_LAST);
    }
    {
        int count = world->m_spawns.size();
        Node* node = static_cast<ToggleNode*>(&world->m_spawns);
        HTREEITEM group = node ? insertCell(new Cell(node), (HTREEITEM)m_64, 5, count, TVI_LAST) : 0;
        for (int i = 0; i < count; ++i)
            if (Node* child = world->m_spawns.m_items[i])
                insertCell(new Cell(child), group, 4, 0, TVI_LAST);
    }
    {
        int count = world->m_namedAreas.size();
        Node* node = static_cast<ToggleNode*>(&world->m_namedAreas);
        HTREEITEM group = node ? insertCell(new Cell(node), (HTREEITEM)m_64, 5, count, TVI_LAST) : 0;
        for (int i = 0; i < count; ++i)
            if (Node* child = world->m_namedAreas.m_items[i])
                insertCell(new Cell(child), group, 4, 0, TVI_LAST);
    }
    {
        int count = world->m_npcs.size();
        Node* node = static_cast<ToggleNode*>(&world->m_npcs);
        HTREEITEM group = node ? insertCell(new Cell(node), (HTREEITEM)m_64, 5, count, TVI_LAST) : 0;
        for (int i = 0; i < count; ++i)
            if (Node* child = world->m_npcs.m_items[i])
                insertCell(new Cell(child), group, 4, 0, TVI_LAST);
    }

    insertSounds((LPARAM)m_64, &world->m_musics);
    insertSounds((LPARAM)m_64, &world->m_sounds);
}

void WorldList::insertSounds(LPARAM parent, kWorld::Sounds* sounds)
{
    kWorld::Sounds::Items& items = sounds->m_items;
    int count = items.size();
    HTREEITEM group = insertCell(new Cell(sounds), (HTREEITEM)parent, 6, count, TVI_LAST);
    if (!group)
        return;
    sounds->m_data = (int)(INT_PTR)group;
    for (int i = 0; i < count; ++i)
    {
        kWorld::Sounds::Item* entry = items.get(i);
        if (!entry)
            continue;
        if (entry->m_type == 9)
        {
            int n = entry->m_zoneAreas.size();
            HTREEITEM sub = insertCell(new Cell(entry), group, 7, n, TVI_LAST);
            for (int j = 0; j < n; ++j)
                if (kWorld::Sounds::ZoneArea* child = entry->m_zoneAreas.get(j))
                    insertCell(new Cell(child), sub, 8, 0, TVI_LAST);
        }
        else
        {
            int n = entry->m_spotAreas.size();
            HTREEITEM sub = insertCell(new Cell(entry), group, 7, n, TVI_LAST);
            for (int j = 0; j < n; ++j)
                if (kWorld::Sounds::SpotArea* child = entry->m_spotAreas.get(j))
                    insertCell(new Cell(child), sub, 8, 0, TVI_LAST);
        }
    }
}

void WorldList::insertItems(LPARAM parent, kWorld::Element* element)
{
    kWorld::Element::Items& items = element->m_items;
    int count = items.size();
    HTREEITEM group = insertCell(new Cell(element), (HTREEITEM)parent, 2, count, TVI_LAST);
    if (!group)
        return;
    element->m_data = (int)(INT_PTR)group;
    for (int i = 0; i < count; ++i)
    {
        kWorld::Element::Item* entry = items.get(i);
        if (!entry)
            continue;
        int n = entry->m_coords.size();
        HTREEITEM sub = insertCell(new Cell(entry), group, 3, n, TVI_LAST);
        for (int j = 0; j < n; ++j)
            if (kWorld::Element::Coord* coord = entry->m_coords.m_items[j])
                insertCell(new Cell(coord), sub, 4, 0, TVI_LAST);
    }
}

static const wchar_t kDungeonFileFilter[] = L"Dungeon File\0*.dg\0";
static const wchar_t kSmodFilesFilter[]   = L"SMOD Files\0*.smod\0";
static const wchar_t kManiFilesFilter[]   = L"MANI Files\0*.mani\0";
static const wchar_t kVaniFilesFilter[]   = L"VANI Files\0*.vani\0";
static const wchar_t kWavFilesFilter[]    = L"WAV Files\0*.wav\0";

bool WorldList::editNode(Node* node)
{
    WorldView* view = WL_VIEW;
    view->setStatusText(0, 0);

    if (dynamic_cast<kWorld::Portals*>(node))
    {
        kWorld::Portals& list = dynamic_cast<kWorld::Portals&>(*node);
        bool ok;
        {
            kWorld::Portal portal(list.size());
            WldPortalEditor editor(view, &portal);
            if (!editor.create(m_hWnd) || editor.doModal() == 1)
            {
                SetFocus(WL_TREE);
                kWorld::Portal* copy = new kWorld::Portal(portal);
                list.add(copy);
                HTREEITEM parent = (HTREEITEM)(INT_PTR)list.m_data;
                HTREEITEM item = copy ? insertCell(new Cell(copy), parent, 4, 0, TVI_LAST) : 0;
                SendMessageW(WL_TREE, TVM_SELECTITEM, TVGN_CARET, (LPARAM)item);
                onItemsChanged();
                ok = true;
            }
            else
            {
                ok = false;
            }
        }
        return ok;
    }

    if (dynamic_cast<kWorld::Spawns*>(node))
    {
        kWorld::Spawns& list = dynamic_cast<kWorld::Spawns&>(*node);
        bool ok;
        {
            kWorld::Spawn spawn(list.size());
            SpawnAreaEditor editor(view, &spawn);
            if (!editor.create(m_hWnd) || editor.doModal() == 1)
            {
                SetFocus(WL_TREE);
                kWorld::Spawn* copy = new kWorld::Spawn(spawn);
                list.add(copy);
                HTREEITEM parent = (HTREEITEM)(INT_PTR)list.m_data;
                HTREEITEM item = copy ? insertCell(new Cell(copy), parent, 4, 0, TVI_LAST) : 0;
                SendMessageW(WL_TREE, TVM_SELECTITEM, TVGN_CARET, (LPARAM)item);
                onItemsChanged();
                ok = true;
            }
            else
            {
                ok = false;
            }
        }
        return ok;
    }

    if (dynamic_cast<kWorld::NamedAreas*>(node))
    {
        kWorld::NamedAreas& list = dynamic_cast<kWorld::NamedAreas&>(*node);
        bool ok;
        {
            kWorld::NamedArea area(list.size());
            area.reset();
            NamedAreaEditor editor(view, &area);
            if (!editor.create(m_hWnd) || editor.doModal() == 1)
            {
                SetFocus(WL_TREE);
                kWorld::NamedArea* copy = new kWorld::NamedArea(area);
                list.add(copy);
                HTREEITEM item = insertNode((HTREEITEM)(INT_PTR)list.m_data, copy, 4, 0, TVI_LAST);
                SendMessageW(WL_TREE, TVM_SELECTITEM, TVGN_CARET, (LPARAM)item);
                onItemsChanged();
                ok = true;
            }
            else
            {
                ok = false;
            }
        }
        return ok;
    }

    if (dynamic_cast<kWorld::NPCs*>(node))
    {
        kWorld::NPCs& list = dynamic_cast<kWorld::NPCs&>(*node);
        bool ok;
        {
            kWorld::NPC npc(list.size());
            npc.reset();
            WldNpcEditor editor(view, &npc);
            if (!editor.create(m_hWnd) || editor.doModal() == 1)
            {
                SetFocus(WL_TREE);
                kWorld::NPC* copy = new kWorld::NPC(npc);
                list.add(copy);
                HTREEITEM item = insertNode((HTREEITEM)(INT_PTR)list.m_data, copy, 4, 0, TVI_LAST);
                SendMessageW(WL_TREE, TVM_SELECTITEM, TVGN_CARET, (LPARAM)item);
                onItemsChanged();
                ok = true;
            }
            else
            {
                ok = false;
            }
        }
        return ok;
    }

    String root;
    C2_STUDIO->getWorkspacePath(&root);

    if (dynamic_cast<kWorld::Sounds*>(node))
    {
        if (C2_STUDIO->isArchiveReadOnly())
            return false;
        kWorld::Sounds& sounds = dynamic_cast<kWorld::Sounds&>(*node);
        root << L"Sound\\";
        root << (sounds.m_index != 9 ? L"Back\\" : L"Music\\");
        String file(root);
        if (!FileDialogs_open(m_hWnd, &file, kWavFilesFilter, L"Choose a file to be inserted", false))
            return false;
        if (!file.containsNoCase(root.m_buffer))
        {
            view->setStatusText(L"The file does not belong to the right entity folder.", 0);
            MessageBeep(MB_ICONEXCLAMATION);
            return false;
        }
        LPWSTR name = PathFindFileNameW(file.m_buffer);
        if (sounds.hasItem(name))
        {
            view->setStatusText(L"The selected file is already present in the list.", 0);
            MessageBeep(MB_ICONEXCLAMATION);
            return false;
        }
        Node* added = sounds.addItem(name);
        HTREEITEM item = insertNode((HTREEITEM)(INT_PTR)sounds.m_data, added, 7, 0, TVI_LAST);
        setFirstVisible((LPARAM)item);
        onItemsChanged();
        return true;
    }

    if (dynamic_cast<kWorld::Sounds::Item*>(node))
    {
        kWorld::Sounds::Item& owner = dynamic_cast<kWorld::Sounds::Item&>(*node);
        if (owner.m_type == 9)
        {
            {
                kWorld::Sounds::ZoneArea zone;
                ZoneAreaEditor editor(view, &zone);
                if (editor.create(m_hWnd) && editor.doModal() != 1)
                    return false;
                kWorld::Sounds::ZoneArea* copy = new kWorld::Sounds::ZoneArea(zone);
                owner.m_zoneAreas.add(copy);
                copy->m_index = owner.m_zoneAreas.size();
                HTREEITEM item = insertNode((HTREEITEM)(INT_PTR)owner.m_data, copy, 8, 0, TVI_LAST);
                setFirstVisible((LPARAM)item);
            }
            onItemsChanged();
            return true;
        }
        {
            kWorld::Sounds::SpotArea spot;
            SpotAreaEditor editor(view, &spot);
            if (editor.create(m_hWnd) && editor.doModal() != 1)
                return false;
            kWorld::Sounds::SpotArea* copy = new kWorld::Sounds::SpotArea(spot);
            owner.m_spotAreas.add(copy);
            copy->m_index = owner.m_spotAreas.size();
            HTREEITEM item = insertNode((HTREEITEM)(INT_PTR)owner.m_data, copy, 8, 0, TVI_LAST);
            setFirstVisible((LPARAM)item);
        }
        onItemsChanged();
        return true;
    }

    if (dynamic_cast<kWorld::Element*>(node))
    {
        if (C2_STUDIO->isArchiveReadOnly())
            return false;
        kWorld::Element& element = dynamic_cast<kWorld::Element&>(*node);
        const wchar_t* folder;
        if (element.m_index == 6)
        {
            folder = L"world\\dungeon";
        }
        else
        {
            root << L"Entity\\";
            if (element.m_index == 5)
                folder = kWorld::g_sectionNames[4];
            else
                folder = kWorld::g_sectionNames[element.m_index];
        }
        root << folder;
        root << '\\';

        const wchar_t* filter = kSmodFilesFilter;
        switch (element.m_index)
        {
        case 6: filter = kDungeonFileFilter; break;
        case 4:
        case 5: filter = kVaniFilesFilter; break;
        case 7: filter = kManiFilesFilter; break;
        }
        String file(root);
        if (!FileDialogs_open(m_hWnd, &file, filter, L"Choose a file to be inserted", false))
            return false;
        if (!file.containsNoCase(root.m_buffer))
        {
            view->setStatusText(L"The file does not belong to the right entity folder.", 0);
            MessageBeep(MB_ICONEXCLAMATION);
            return false;
        }
        LPWSTR name = PathFindFileNameW(file.m_buffer);
        if (element.hasItem(name))
        {
            view->setStatusText(L"The selected file is already present in the list.", 0);
            MessageBeep(MB_ICONEXCLAMATION);
            return false;
        }
        Node* added = element.addItem(name);
        HTREEITEM item = insertNode((HTREEITEM)(INT_PTR)element.m_data, added, 3, 0, TVI_LAST);
        setFirstVisible((LPARAM)item);
        onItemsChanged();
        return true;
    }

    if (dynamic_cast<kWorld::Element::Item*>(node))
    {
        kWorld::Element::Item& owner = dynamic_cast<kWorld::Element::Item&>(*node);
        kWorld::Element::Coord coord(owner.m_mode);
        coord.m_scale.y = 1.0f;
        WorldEditor* editor;
        if (coord.m_mode == 1)
            editor = new CoordsEditor(view, &coord);
        else
            editor = new CoordsEditor2(view, &coord);
        bool result = false;
        if (editor->create(m_hWnd) && editor->doModal() == 1)
        {
            kWorld::Element::Coord* copy = new kWorld::Element::Coord(coord);
            owner.m_coords.add(copy);
            HTREEITEM item = insertNode((HTREEITEM)(INT_PTR)owner.m_data, copy, 4, 0, TVI_LAST);
            setFirstVisible((LPARAM)item);
            onItemsChanged();
            result = true;
        }
        delete editor;
        return result;
    }

    if (!dynamic_cast<Name*>(node))
        return false;
    Name& name = dynamic_cast<Name&>(*node);
    if (name.m_id != 11)
        return false;
    kWorld::Items* mesh = (kWorld::Items*)(INT_PTR)name.m_owner;
    if (!mesh)
        return false;
    kWorld::Vertices& vertices = mesh->m_vertices;
    bool result;
    {
        kWorld::Vertice vertice(vertices.size());
        vertice.m_rot.z = 1.0f;
        vertice.m_p3.y = 1.0f;
        VerticeEditor editor(view, &vertice);
        if (!editor.create(m_hWnd) || editor.doModal() == 1)
        {
            kWorld::Vertice* copy = new kWorld::Vertice(vertice);
            vertices.add(copy);
            HTREEITEM item = insertNode((HTREEITEM)(INT_PTR)name.m_data, copy, 4, 0, TVI_LAST);
            setFirstVisible((LPARAM)item);
            onItemsChanged();
            result = true;
        }
        else
        {
            result = false;
        }
    }
    return result;
}

bool WorldList::removeChildNode(Node* parent, HTREEITEM item)
{
    Node* node = getItemNode(item);
    if (!node)
        return false;

    if (dynamic_cast<kWorld::Portals*>(parent))
    {
        kWorld::Portals& list = dynamic_cast<kWorld::Portals&>(*parent);
        if (kWorld::Portal* portal = dynamic_cast<kWorld::Portal*>(node))
            return list.detachItem(portal);
        return false;
    }
    if (dynamic_cast<kWorld::Spawns*>(parent))
    {
        kWorld::Spawns& list = dynamic_cast<kWorld::Spawns&>(*parent);
        if (kWorld::Spawn* spawn = dynamic_cast<kWorld::Spawn*>(node))
            return list.detachItem(spawn);
        return false;
    }
    if (dynamic_cast<kWorld::NamedAreas*>(parent))
    {
        kWorld::NamedAreas& list = dynamic_cast<kWorld::NamedAreas&>(*parent);
        if (kWorld::NamedArea* area = dynamic_cast<kWorld::NamedArea*>(node))
            return list.detachItem(area);
        return false;
    }
    if (dynamic_cast<kWorld::NPCs*>(parent))
    {
        kWorld::NPCs& list = dynamic_cast<kWorld::NPCs&>(*parent);
        if (kWorld::NPC* npc = dynamic_cast<kWorld::NPC*>(node))
            return list.detachItem(npc);
        return false;
    }
    if (dynamic_cast<kWorld::Element::Item*>(node))
    {
        kWorld::Element::Item* entry = dynamic_cast<kWorld::Element::Item*>(node);
        kWorld::Element* element = dynamic_cast<kWorld::Element*>(parent);
        if (!element)
            return false;
        if (entry->m_coords.size() > 0 &&
            AlertDial::alert(m_hWnd, L"Are you sure you want to delete this SMOD file and all its locations?", 0,
                             MB_YESNO | MB_ICONQUESTION) != IDYES)
            return false;
        return element->m_items.detachItem(entry);
    }
    if (dynamic_cast<kWorld::Element::Coord*>(node))
    {
        kWorld::Element::Coord* coord = dynamic_cast<kWorld::Element::Coord*>(node);
        kWorld::Element::Item* entry = dynamic_cast<kWorld::Element::Item*>(parent);
        if (!entry)
            return false;
        return entry->removeCoord(coord);
    }
    if (dynamic_cast<kWorld::Sounds::Item*>(node))
    {
        kWorld::Sounds::Item* entry = dynamic_cast<kWorld::Sounds::Item*>(node);
        kWorld::Sounds* sounds = dynamic_cast<kWorld::Sounds*>(parent);
        if (!sounds)
            return false;
        int places = entry->m_type == 9 ? entry->m_zoneAreas.size() : entry->m_spotAreas.size();
        if (places > 0 &&
            AlertDial::alert(m_hWnd, L"Are you sure you want to delete this WAV file and all its locations?", 0,
                             MB_YESNO | MB_ICONQUESTION) != IDYES)
            return false;
        return sounds->removeItem(entry);
    }
    if (dynamic_cast<kWorld::Sounds::ZoneArea*>(node))
    {
        if (!parent)
            return false;
        kWorld::Sounds::ZoneArea* zone = dynamic_cast<kWorld::Sounds::ZoneArea*>(node);
        ptrArray<kWorld::Sounds::ZoneArea*>& zones = static_cast<kWorld::Sounds::Item*>(parent)->m_zoneAreas;
        if (!zones.detachItem(zone))
            return false;
        zones.destroyItem(zone);
        return true;
    }
    if (dynamic_cast<kWorld::Sounds::SpotArea*>(node))
    {
        if (!parent)
            return false;
        kWorld::Sounds::SpotArea* spot = dynamic_cast<kWorld::Sounds::SpotArea*>(node);
        ptrArray<kWorld::Sounds::SpotArea*>& spots = static_cast<kWorld::Sounds::Item*>(parent)->m_spotAreas;
        if (!spots.detachItem(spot))
            return false;
        spots.destroyItem(spot);
        return true;
    }
    if (dynamic_cast<kWorld::Vertice*>(node))
    {
        dynamic_cast<kWorld::Vertice*>(node);
        Name* name = dynamic_cast<Name*>(parent);
        if (!name)
            return false;
        kWorld::Items* mesh = (kWorld::Items*)(INT_PTR)name->m_owner;
        if (!mesh)
            return false;
        kWorld::Vertice* vertice = dynamic_cast<kWorld::Vertice*>(node);
        kWorld::Vertices& vertices = mesh->m_vertices;
        if (!vertices.detachItem(vertice))
            return false;
        vertices.destroyItem(vertice);
        return true;
    }
    return false;
}

int WorldList::getItemMenu(HTREEITEM item)
{
    Name*& menuName = reinterpret_cast<Name*&>(m_selected);
    menuName = dynamic_cast<Name*>(getItemNode(item));
    if (!menuName)
        return 0;
    LPCWSTR ext = PathFindExtensionW(menuName->m_file.m_buffer);
    if (!ext)
        return 0;
    String e;
    e.append(ext, (unsigned)-1);
    if (e == L".dg" || e == L".smod" || e == L".wav" || e == L".vani" || e == L".mani")
        return 195;
    return 0;
}

bool WorldList::onCommand(UINT code, HWND ctrl, UINT id)
{
    if ((code | 1) == 1 && !ctrl)
    {
        if (id == 0x44E)
        {
            if (g_studioFrame && !C2_STUDIO->isArchiveReadOnly())
            {
                Name* menuName = reinterpret_cast<Name*&>(m_selected);
                kWorld::Section* section = (kWorld::Section*)(INT_PTR)menuName->m_owner;
                if (section)
                {
                    String path;
                    C2_STUDIO->getWorkspacePath(&path);
                    const wchar_t* filter = kSmodFilesFilter;
                    switch (section->m_index)
                    {
                    case 9:
                        path << L"Sound\\Music\\";
                        filter = kWavFilesFilter;
                        break;
                    case 10:
                        path << L"Sound\\Back\\";
                        filter = kWavFilesFilter;
                        break;
                    case 6:
                        path << L"world\\dungeon\\";
                        filter = kDungeonFileFilter;
                        break;
                    default:
                        path << L"Entity\\";
                        path << (section->m_index == 5 ? kWorld::g_sectionNames[4] : kWorld::g_sectionNames[section->m_index]);
                        path << '\\';
                        if (section->m_index == 4 || section->m_index == 5)
                            filter = kVaniFilesFilter;
                        else if (section->m_index == 7)
                            filter = kManiFilesFilter;
                        break;
                    }
                    String folder(path);
                    if (FileDialogs_open(m_hWnd, &path, filter, L"Choose the substitution file", false))
                    {
                        LPWSTR name = PathFindFileNameW(path.m_buffer);
                        if (!path.containsNoCase(folder.m_buffer))
                        {
                            WL_VIEW->setStatusText(L"The file does not belong to the right entity folder.", 0);
                            MessageBeep(MB_ICONEXCLAMATION);
                        }
                        else if (section->hasItem(name))
                        {
                            WL_VIEW->setStatusText(L"The selected file is already present in the list.", 0);
                            MessageBeep(MB_ICONEXCLAMATION);
                        }
                        else
                        {
                            reinterpret_cast<Name*&>(m_selected)->setString('name', name);
                            invalidateItem(reinterpret_cast<Name*&>(m_selected)->m_data);
                        }
                    }
                }
            }
            return true;
        }
        if (id == 0x44F)
            return true;
    }
    return TreeList::onCommand(code, ctrl, id);
}
