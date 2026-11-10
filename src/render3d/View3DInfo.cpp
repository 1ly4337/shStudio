#include "View3DInfo.h"
#include "View3DInfo_Coord.h"
#include "_imports_p06.h"

View3DInfo::View3DInfo(Drawable* model)
    : Modal(), m_coordsList(), m_itemsList(), m_model(model)
{
}

View3DInfo::~View3DInfo()
{
}

View3DInfo::CoordsList::~CoordsList()
{
}

int View3DInfo::CoordsList::indexOfNode(Node* node)
{
    return m_coords.indexOf(dynamic_cast<View3DInfo::Coord*>(node));
}
