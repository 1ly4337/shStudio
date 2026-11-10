#include "ItemsTab.h"
#include "ItemView.h"
#include "LightGearList.h"
#include "DarkGearList.h"
#include "LightWeaponList.h"
#include "DarkWeaponList.h"
#include "AccessoryList.h"
#include "LapisList.h"
#include "CapeList.h"
#include "MountList.h"
#include "GoodsList.h"
#include "PetsList.h"
#include "WingList.h"
#include "CustomList.h"
#include "RawItemList.h"
#include "GradeList.h"
#include "Data_Item.h"
#include "Data_Items.h"
#include "Data_Item_fields_B.h"
#include "Exception.h"
#include "StudioFrame.h"
#include "_imports_A.h"

static const wchar_t* g_itemTabTitles[14] = {
    L" Light Gears ", L" Dark Gears ", L" Light Weapons ", L" Dark Weapons ", L" Accessories ",
    L" Lapis ", L" Capes ", L" Mounts ", L" Consumables ", L" Pets ", L" Wings ", L" Costumes ",
    L" All Items ", L" Grades ",
};

static void applyItemIcons(ItemList* list)
{
    ImageList* icons = ((StudioFrame*)g_studioFrame)->m_client.m_clientData.getImageList();
    if (ImageList_GetImageCount(icons->m_hList) > 0)
        list->setSmallImages(icons);
}

void ItemsTab::onActivate(bool active)
{
    ItemList* page = pageAt(-1);
    if (!page)
        return;
    if (page == (ItemList*)m_grades)
        ((Window*)m_grades)->activate(active);
    else
        page->onActivate(active);
}

HWND ItemsTab::getPage(int index)
{
    ItemList* page = pageAt(index);
    if (page == (ItemList*)m_grades)
        return ((Window*)m_grades)->m_hWnd;
    if (page)
        return page->m_hWnd;
    return 0;
}

bool ItemsTab::createPages(RECT* rc)
{
    if (!m_items)
        return false;

    int created = 1;
    if (addPage(new LightGearList(m_view, m_items), 'ltgr', g_itemTabTitles[0], -1))
    {
        created = 2;
        if (addPage(new DarkGearList(m_view, m_items), 'drgr', g_itemTabTitles[1], -1))
        {
            created = 3;
            if (addPage(new LightWeaponList(m_view, m_items), 'ltwp', g_itemTabTitles[2], -1))
            {
                created = 4;
                if (addPage(new DarkWeaponList(m_view, m_items), 'drwp', g_itemTabTitles[3], -1))
                {
                    AccessoryList* accessories = new AccessoryList(m_view, m_items);
                    bool ok = addPage(accessories, 'accs', g_itemTabTitles[4], -1);
                    if (ok)
                    {
                        applyItemIcons(accessories);
                        ok = true;
                    }
                    created = 5;
                    if (ok)
                    {
                        LapisList* lapis = new LapisList(m_view, m_items);
                        ok = addPage(lapis, 'laps', g_itemTabTitles[5], -1);
                        if (ok)
                        {
                            applyItemIcons(lapis);
                            ok = true;
                        }
                        created = 6;
                        if (ok)
                        {
                            CapeList* capes = new CapeList(m_view, m_items);
                            ok = addPage(capes, 'cape', g_itemTabTitles[6], -1);
                            if (ok)
                            {
                                applyItemIcons(capes);
                                ok = true;
                            }
                            created = 7;
                            if (ok)
                            {
                                MountList* mounts = new MountList(m_view, m_items);
                                ok = addPage(mounts, 'mnts', g_itemTabTitles[7], -1);
                                if (ok)
                                {
                                    applyItemIcons(mounts);
                                    ok = true;
                                }
                                created = 8;
                                if (ok)
                                {
                                    GoodsList* goods = new GoodsList(m_view, m_items);
                                    ok = addPage(goods, 'good', g_itemTabTitles[8], -1);
                                    if (ok)
                                    {
                                        applyItemIcons(goods);
                                        ok = true;
                                    }
                                    created = 9;
                                    if (ok)
                                    {
                                        PetsList* pets = new PetsList(m_view, m_items);
                                        ok = addPage(pets, 'pets', g_itemTabTitles[9], -1);
                                        if (ok)
                                        {
                                            applyItemIcons(pets);
                                            ok = true;
                                        }
                                        created = 10;
                                        if (ok)
                                        {
                                            WingList* wings = new WingList(m_view, m_items);
                                            ok = addPage(wings, 'wing', g_itemTabTitles[10], -1);
                                            if (ok)
                                            {
                                                applyItemIcons(wings);
                                                ok = true;
                                            }
                                            created = 11;
                                            if (ok)
                                            {
                                                CustomList* costumes = new CustomList(m_view, m_items);
                                                ok = addPage(costumes, 'cost', g_itemTabTitles[11], -1);
                                                if (ok)
                                                {
                                                    applyItemIcons(costumes);
                                                    ok = true;
                                                }
                                                created = 12;
                                                if (ok)
                                                {
                                                    addPage(new RawItemList(m_view, m_items), 'raw ', g_itemTabTitles[12], -1);
                                                    created = 13;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    m_grades = new GradeList(m_view, m_items);
    if (m_grades)
    {
        if (m_grades->Window::create(m_hWnd, (HMENU)'grde', rc, WS_CHILD, 0, 0))
        {
            insertPage(g_itemTabTitles[created], (HWND)m_grades, (LPARAM)m_grades, -1);
        }
        else
        {
            delete m_grades;
            m_grades = 0;
        }
    }
    return true;
}

void ItemsTab::reloadAll()
{
    int count = m_pages.m_count;
    for (int i = 0; i < count; ++i)
    {
        if (i < 0 || i >= m_pages.m_capacity)
            throw ArrayIndexOutOfBoundsException(i, m_pages.m_capacity);
        ItemList* page = m_pages.m_table[i].value;
        if (page)
        {
            page->setItemCount(0);
            for (int j = 0; j < page->m_set.m_count; ++j)
                page->m_set.m_items[j] = 0;
            page->m_set.m_count = 0;
            page->loadItems(m_items);
            page->setItemCount(page->m_set.size());
        }
    }
    if (m_grades)
        m_grades->refresh();
}

void ItemsTab::setGrade(short grade)
{
    ItemList* page = pageAt(-1);
    if (!page)
        return;
    for (int i = 0; ; ++i)
    {
        if (!IsWindow(page->m_hWnd))
            return;
        if (page->getCount(1) < 1)
            return;
        int count = page->getCount(0);
        if (i >= count)
            return;
        while (SendMessageW(page->m_hWnd, LVM_GETITEMSTATE, i, LVIS_SELECTED) != LVIS_SELECTED)
        {
            if (++i >= count)
                return;
        }
        if (i == -1)
            return;
        Data::Item* item = dynamic_cast<Data::Item*>(page->getNode(i));
        if (item)
            DI_Grade(item) = grade;
    }
}
