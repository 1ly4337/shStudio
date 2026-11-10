#include "MetaStruct3D.h"
#include "_imports_p06.h"

MetaStruct3D::Structs3D::Structs3D()
    : ptrArray<Struct3D*>(256)
{
}

MetaStruct3D::~MetaStruct3D()
{
    m_parts.deleteAll();
    m_center.z = 0;
    m_center.y = 0;
    m_center.x = 0;
    m_min.z = 0;
    m_min.y = 0;
    m_min.x = 0;
    m_max.z = 0;
    m_max.y = 0;
    m_max.x = 0;
    m_texture = 0;
}

BOOL MetaStruct3D::isEmpty()
{
    return m_parts.size() == 0;
}

bool MetaStruct3D::addPart(Struct3D* part)
{
    return m_parts.add(part);
}
