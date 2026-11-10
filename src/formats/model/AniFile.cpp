#include "p05_common.h"
#include "AniFile.h"

AniFile::~AniFile()
{
    m_steps.deleteAll();
}
