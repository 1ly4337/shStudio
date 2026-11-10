#include "p05_common.h"
#include "Effect.h"

Effect::Effect()
    : Serializable(), m_names(16), m_textures(16), m_scenes(), m_sequences()
{
    m_flags = 0;
}

Effect::~Effect()
{
}

bool Effect::isEmpty() const
{
    return m_scenes.size() == 0;
}

void Effect::write(oStream& , DWORD ) const
{
    throw 0;
}
