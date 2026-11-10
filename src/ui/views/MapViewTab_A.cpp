#include "MapViewTab.h"
#include "MapSpots.h"
#include "SpotDetails.h"
#include "_imports_p06.h"

void MapViewTab::onPageChanged(int index)
{
    m_spots->destroySpots();
    SpotDetails* page = pageAt(index);
    if (page)
        page->showSpots();
}
