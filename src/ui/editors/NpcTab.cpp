#include "NpcTab.h"
#include "NpcView.h"
#include "NpcData.h"
#include "MerchantList.h"
#include "GateKeeperList.h"
#include "StandardsList.h"
#include "QuestsList.h"
#include "ida_compat.h"

void NpcTab::onActivate(bool active)
{
    NpcList* page = pageAt(-1);
    if (page)
    {
        page->onActivate(active);
        page->invalidate(true);
    }
}

bool NpcTab::createPages(RECT* rc)
{
    if (!m_data)
        return false;
    addPage(new MerchantList(m_view, m_data), 'merc', L"Merchants", -1);
    addPage(new GateKeeperList(m_view, m_data), 'gate', L"Gate-Keepers", -1);
    addPage(new StandardsList(m_view, m_data), 'stds', L"Standard-NPCs", -1);
    addPage(new QuestsList(m_view, m_data), 'qust', L"Quests", -1);
    return true;
}

void NpcTab::fillLists()
{
    if (!m_data)
        return;
    NpcList* page = getPageByKey('merc');
    if (page)
        page->setItemCount(m_data->m_npcs[1].size());
    page = getPageByKey('gate');
    if (page)
        page->setItemCount(m_data->m_npcs[2].size());
    page = getPageByKey('qust');
    if (page)
        page->setItemCount(m_data->m_quests.size());
    StandardsList* standards = dynamic_cast<StandardsList*>(getPageByKey('stds'));
    if (standards)
        standards->refill();
}
