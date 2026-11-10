#include "WorldDropManager.h"
#include "World.h"
#include "kWorld_Element.h"
#include "kWorld_Sounds.h"
#include "String.hpp"
#include "StudioFrame.h"
#include "TreeList.h"
#include "world_common.h"
#include <commctrl.h>

#define WDM_TREELIST(owner) (reinterpret_cast<TreeList*>(owner))

WorldDropManager::WorldDropManager(WorldList* owner, World* world)
    : DropTargetMgr(WDM_TREELIST(owner)->m_tree.m_hWnd)
{
    m_world  = world;
    m_owner  = owner;
    m_tree   = WDM_TREELIST(owner)->m_tree.m_hWnd;
    m_kind   = -1;
    m_hilite = 0;

    WORD format = (WORD)g_cfWorldFile;
    FORMATETC* fmt = (FORMATETC*)operator new(sizeof(FORMATETC));
    fmt->cfFormat = format;
    fmt->ptd      = 0;
    fmt->dwAspect = DVASPECT_CONTENT;
    fmt->lindex   = -1;
    fmt->tymed    = TYMED_HGLOBAL;
    m_formats.add(fmt);
}

WorldDropManager::~WorldDropManager()
{
}

void WorldDropManager::onDragLeave()
{
    m_kind = -1;
    SendMessageW(m_tree, TVM_SELECTITEM, TVGN_DROPHILITE, 0);
}

bool WorldDropManager::splitPath(String& path, String& head)
{
    unsigned length = path.m_length;
    unsigned pos = 0;
    if (!length)
        return false;
    for (const wchar_t* p = path.m_buffer; *p != L'\\'; ++p)
    {
        if (++pos >= length)
            return false;
    }
    if (pos == (unsigned)-1)
        return false;

    head = path.left(pos);
    head.toLowerInPlace();
    path = path.right(path.m_length - pos - 1);
    return true;
}

kWorld::Section* WorldDropManager::findSection(String& path, kWorld::Items* world, int* kind)
{
    kWorld::Section* section = 0;
    int type = -1;
    if (kind)
        *kind = -1;

    String head;
    if (splitPath(path, head))
    {
        if (head == L"building")
        {
            type = 0;
            section = &world->m_building;
        }
        else if (head == L"grass")
        {
            type = 3;
            section = &world->m_grass;
        }
        else if (head == L"mani")
        {
            type = 7;
            section = &world->m_mani;
        }
        else if (head == L"object")
        {
            type = 8;
            section = &world->m_object;
        }
        else if (head == L"shape")
        {
            type = 1;
            section = &world->m_shape;
        }
        else if (head == L"tree")
        {
            type = 2;
            section = &world->m_tree;
        }
        else if (head == L"vani")
        {
            type = 4;
            section = &world->m_vani;
        }
        if (kind)
            *kind = type;
    }
    else
    {
        section = 0;
    }
    return section;
}

bool WorldDropManager::onDragOver(DWORD keyState, POINT* clientPt, DWORD* effect)
{
    if (!*effect)
    {
        SendMessageW(m_tree, TVM_SELECTITEM, TVGN_DROPHILITE, 0);
        m_hilite = 0;
        m_kind = -1;
        return false;
    }
    if (!DropTargetMgr::onDragOver(keyState, clientPt, effect))
        return false;

    if (m_kind != -1 || m_format->cfFormat != g_cfWorldFile || m_medium.tymed != TYMED_HGLOBAL)
        return true;

    SendMessageW(m_tree, TVM_SELECTITEM, TVGN_DROPHILITE, 0);
    String path;
    path.assign(m_medium.hGlobal);
    if (!C2_STUDIO->findFile(path.m_buffer))
        return false;

    bool result = false;
    String head;
    if (splitPath(path, head))
    {
        kWorld::Section* section = 0;
        bool present = true;
        if (head == L"entity")
        {
            section = findSection(path, m_world->getItems(), &m_kind);
            if (section)
                present = section->hasItem(path.m_buffer);
        }
        else if ((head == L"sound") && splitPath(path, head))
        {
            if (head == L"back")
            {
                m_kind = 10;
                section = &m_world->getItems()->m_sounds;
            }
            else if (head == L"music")
            {
                m_kind = 9;
                section = &m_world->getItems()->m_musics;
            }
            if (section)
                present = section->hasItem(path.m_buffer);
        }
        if (section && !present)
        {
            LPARAM item = section->m_data;
            m_hilite = item;
            SendMessageW(m_tree, TVM_SELECTITEM, TVGN_DROPHILITE, item);
            *effect = DROPEFFECT_COPY;
            result = true;
        }
    }
    return result;
}

bool WorldDropManager::onDrop(FORMATETC* format, STGMEDIUM* medium, DWORD* effect)
{
    m_hilite = 0;
    m_kind = -1;
    SendMessageW(m_tree, TVM_SELECTITEM, TVGN_DROPHILITE, 0);
    if (format->cfFormat != g_cfWorldFile || medium->tymed != TYMED_HGLOBAL)
        return false;

    String path;
    path.assign(medium->hGlobal);
    if (!C2_STUDIO->findFile(path.m_buffer))
        return false;

    String head;
    if (!splitPath(path, head))
        return false;

    if (head == L"entity")
    {
        kWorld::Section* section = findSection(path, m_world->getItems(), 0);
        if (!section)
            return false;
        if (!section->hasItem(path.m_buffer))
        {
            Node* added = section->addItem(path.m_buffer);
            HTREEITEM item = WDM_TREELIST(m_owner)->insertNode((HTREEITEM)section->m_data, added, 3, 0, TVI_LAST);
            WDM_TREELIST(m_owner)->setFirstVisible((LPARAM)item);
            WDM_TREELIST(m_owner)->onItemsChanged();
        }
    }
    else
    {
        if (!(head == L"sound") || !splitPath(path, head))
            return false;
        kWorld::Section* section;
        if (head == L"back")
            section = &m_world->getItems()->m_sounds;
        else if (head == L"music")
            section = &m_world->getItems()->m_musics;
        else
            return false;
        if (section)
            section->hasItem(path.m_buffer);
    }
    return false;
}
