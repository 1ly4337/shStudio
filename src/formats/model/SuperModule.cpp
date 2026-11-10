#include "SuperModule.h"
#include "_imports_p06.h"

SuperModule::SuperModule()
    : MetaStruct3D(), Serializable()
{
    m_type = 'inle';
    m_A8 = 0.0f;
}

SuperModule::~SuperModule()
{
}

Model3D* SuperModule::loadModel(const String& modelFile, const String& textureFile)
{
    return 0;
}

bool SuperModule::isEmpty() const
{
    return m_parts.size() == 0;
}
