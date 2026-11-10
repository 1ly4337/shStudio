#include "View3DInfo_Coord.h"

const wchar_t* View3DInfo::Coord::getString(DWORD ) const
{
    return m_name;
}

int View3DInfo::Coord::getInt(DWORD tag) const
{
    float value = 0.0f;
    switch (tag) {
    case 'ampl':
        value = m_max - m_min;
        break;
    case 'max ':
        return *(const int*)&m_max;
    case 'min ':
        return *(const int*)&m_min;
    }
    return *(const int*)&value;
}
