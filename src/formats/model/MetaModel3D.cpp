#include "MetaModel3D.h"
#include "_imports_p07.h"
#pragma warning(disable : 4066 4498)

MetaModel3D::MetaModel3D(int )
    : MetaStruct3D()
{
    m_type = 'k3DC';
    m_hObject = 0;
}

MetaModel3D::~MetaModel3D()
{
    if (m_hObject)
        DeleteObject(m_hObject);
}
