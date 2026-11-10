#include "p05_common.h"
#include "Weather.h"

Weather::Weather()
    : Serializable(), m_names(16)
{
    m_level = 0;
    m_current = -1;
    m_flags = 0;
    m_names.deleteAll();
    m_names.m_ids.m_count = 0;
}

Weather::~Weather()
{
}

bool Weather::isEmpty() const
{
    return m_names.size() == 0;
}
