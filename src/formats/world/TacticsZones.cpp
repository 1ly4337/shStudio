#include "p05_common.h"
#include "TacticsZones.h"

TacticsZones::TacticsZones()
    : Collection<TacticsZone>(), Serializable(), m_version(0)
{
}

TacticsZones::~TacticsZones()
{
}
