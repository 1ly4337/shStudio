#include "MetaStruct3D.h"
#include "Struct3D.h"
#include "Model3D.h"
#include "String.hpp"
#include "Exception.h"
#include "ListCtrl.h"
#include "ContentProvider.h"
#include "String_p02.h"
#include "_imports_w3.h"
#include <GL/gl.h>
#include <commctrl.h>
#include <cstdlib>
#include <cfloat>
#include <cstring>

void MetaStruct3D::clear()
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

bool MetaStruct3D::loadTexture(const wchar_t* dir)
{
    int count = m_parts.size();
    for (int i = 0; i < count; ++i)
    {
        Struct3D* part = m_parts.m_items[i];
        if (!part->loadTexture(dir))
            return false;
    }
    m_texture = (GLuint)-1;
    return true;
}

void MetaStruct3D::translate(const fPoint* origin)
{
    int count = m_parts.size();
    for (int i = 0; i < count; ++i)
        m_parts.m_items[i]->translate(origin);
}

void MetaStruct3D::drawWireframe(GLenum , GLenum )
{
    glDisable(GL_BLEND);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(1.0f);
    int count = m_parts.size();
    for (int i = 0; i < count; ++i)
        m_parts.m_items[i]->drawFaces();
}

void MetaStruct3D::drawImmediate()
{
    glShadeModel(GL_SMOOTH);
    glClearDepth(1.0);
    if ((m_flags & 1) != 0)
    {
        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.25f);
    }
    else
    {
        glDisable(GL_ALPHA_TEST);
        glDisable(GL_BLEND);
    }
    int count = m_parts.size();
    if (!m_texture)
    {
        for (int i = 0; i < count; ++i)
            m_parts.m_items[i]->loadTexture(0);
        m_texture = (GLuint)-1;
    }
    for (int i = 0; i < count; ++i)
        m_parts.m_items[i]->drawTextured();
}

bool MetaStruct3D::loadModels(const String& dir, ptrArray<String*>* models, ptrArray<String*>* textures)
{
    clear();
    String modelDir(dir);
    switch (m_type)
    {
    case 0x6B33444F:
        modelDir << "3DO\\";
        break;
    case 0x6B334443:
        modelDir << "3DC\\";
        break;
    case 0x6B334445:
        modelDir << "3DE\\";
        break;
    default:
        return false;
    }

    String textureDir(dir);
    textureDir << "DDS\\";
    int count = models->size();
    for (int i = 0; i < count; ++i)
    {
        Model3D* model;
        {
            if (i < 0 || i >= textures->m_count)
                throw ArrayIndexOutOfBoundsException(i, textures->m_count);
            String texturePath = textureDir + *textures->m_items[i];
            if (i >= models->m_count)
                throw ArrayIndexOutOfBoundsException(i, models->m_count);
            String modelPath = modelDir + *models->m_items[i];
            model = loadModel(modelPath, texturePath);
        }
        if (model)
            m_parts.add(model);
    }
    computeBounds();
    m_texture = (GLuint)-1;
    return true;
}

void MetaStruct3D::computeBounds()
{
    m_min.z = FLT_MAX;
    m_min.y = FLT_MAX;
    m_min.x = FLT_MAX;
    m_max.z = -FLT_MAX;
    m_max.y = -FLT_MAX;
    m_max.x = -FLT_MAX;
    int count = m_parts.size();
    if (count > 0)
    {
        Struct3D** it = m_parts.m_items;
        do
        {
            Struct3D* p = *it;
            if (p)
            {
                if (m_min.x > p->m_min.x) m_min.x = p->m_min.x;
                if (m_min.y > p->m_min.y) m_min.y = p->m_min.y;
                if (m_min.z > p->m_min.z) m_min.z = p->m_min.z;
                if (p->m_max.x > m_max.x) m_max.x = p->m_max.x;
                if (p->m_max.y > m_max.y) m_max.y = p->m_max.y;
                if (p->m_max.z > m_max.z) m_max.z = p->m_max.z;
            }
            ++it;
        } while (--count);
    }
    m_center.x = (m_max.x + m_min.x) * 0.5f;
    m_center.y = (m_max.y + m_min.y) * 0.5f;
    m_center.z = (m_max.z + m_min.z) * 0.5f;
}

void MetaStruct3D::fillProperties(ListCtrl* list)
{
    int faces = 0;
    int vertices = 0;
    int count = m_parts.size();
    for (int i = 0; i < count; ++i)
    {
        faces += ((Struct3D*)m_parts.get(i))->m_faces.size();
        vertices += ((Struct3D*)m_parts.get(i))->m_vertices.size();
    }
    wchar_t buffer[16] = { 0 };
    _ltow_s(count, buffer, 16, 10);
    int row = list->insertTextItem(L"Number of 3D objects", 0, -1);
    if (row >= 0)
    {
        LVITEMW item;
        memset(&item.iItem, 0, 0x38);
        item.pszText = buffer;
        item.mask = LVIF_TEXT;
        item.iItem = row;
        item.iSubItem = 1;
        SendMessageW(list->m_hWnd, LVM_SETITEMW, row, (LPARAM)&item);
    }
    list->insertNumberRow(L"Number of vertices", vertices, 0);
    list->insertNumberRow(L"Number of faces", faces, 0);
}
