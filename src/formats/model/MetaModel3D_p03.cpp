#include "MetaModel3D.h"
#include "Model3D.h"
#include "Bytes.h"
#include "String.hpp"
#include "ContentProvider.h"
#include "String_p02.h"
#include "_imports_w3.h"
#include <cstring>

Model3D* MetaModel3D::loadModel(const String& modelFile, const String& textureFile)
{
    Bytes bytes;
    int err;
    if (g_contentProvider)
        err = g_contentProvider->readPath(textureFile.m_buffer, &bytes);
    else
        err = bytes.load(textureFile.m_buffer);
    if (err)
        return 0;
    HGDIOBJ bitmap = 0;
    if (loadDDS(bytes, &bitmap))
        return 0;

    if (bytes.m_data && bytes.m_capacity)
        memset(bytes.m_data, 0, bytes.m_capacity);
    Model3D* model = 0;
    bytes.m_size = 0;
    bytes.m_pos = 0;

    if (g_contentProvider)
        err = g_contentProvider->readPath(modelFile.m_buffer, &bytes);
    else
        err = bytes.load(modelFile.m_buffer);
    if (!err)
    {
        try
        {
            model = new Model3D(m_type);
            model->read(bytes, 0x66696C65 );
            model->setBitmap((HBITMAP)bitmap);
            return model;
        }
        catch (...)
        {
            delete model;
            model = 0;
        }
    }
    if (bitmap)
        DeleteObject(bitmap);
    return model;
}
