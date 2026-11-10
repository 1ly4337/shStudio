#include "SpotDetails.h"
#include "Window.h"
#include "_imports_p06.h"

SpotDetails::SpotDetails(MapSpots* spots)
    : Container(), m_spots(spots)
{
    m_bkColor = Window_isClassicAppearance() ? GetSysColor(COLOR_BTNFACE) : 0xF0F0F0;
}
