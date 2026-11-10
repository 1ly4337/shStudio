#include "MapHeight.h"
#include <cmath>
#include <cfloat>
#include "iStream.h"
#include "oStream.h"
#include "String.hpp"
#include "ListCtrl.h"
#include "gdiutil.h"
#include "glutils.h"

MapHeight::MapHeight(kWorld::Items* owner)
    : ToggleNode(L"Map-Height")
    , Struct3D()
    , m_owner(owner)
    , m_strings(16)
    , m_textures()
{
    m_mapId = 0;
    m_size = 0;
    m_grid = 0;
    m_texIndex = 0;
    m_tileTextures = 0;
    m_bitmaps = 0;
    m_followEye = false;
    m_drawMode = 0;
}

MapHeight::~MapHeight()
{
    MapHeight::clear();
}

void MapHeight::init(int mapSize)
{
    clear();
    m_size = mapSize / 2 + 1;
    int count = m_size * m_size;
    m_grid = new GridPoint[count];
    m_texIndex = new BYTE[count];
    memset(m_grid, 0, 12 * count);
    memset(m_texIndex, 0, count);
}

void MapHeight::clear()
{
    if (m_grid)
    {
        delete[] m_grid;
        m_grid = 0;
    }
    if (m_texIndex)
    {
        delete[] m_texIndex;
        m_texIndex = 0;
    }
    if (m_tileTextures)
    {
        delete[] m_tileTextures;
        m_tileTextures = 0;
    }
    if (m_bitmaps)
    {
        int count = m_strings.size();
        for (int i = 0; i < count; ++i)
            DeleteObject(m_bitmaps[i]);
        delete[] m_bitmaps;
        m_bitmaps = 0;
    }
    m_strings.deleteAll();
    m_strings.m_ids.m_count = 0;
    for (int i = 0; i < m_textures.m_count; ++i)
        m_textures.clearEntry(&m_textures.m_table[i]);
    m_textures.m_count = 0;
}

void MapHeight::fillProperties(ListCtrl* list)
{
    if (m_textures.m_count > 0)
        list->insertNumberRow(L"Interpolated Textures", m_textures.m_count, 0);
    list->insertNumberRow(L"Number of vertices", m_vertices.size(), 0);
    list->insertNumberRow(L"Number of faces", m_faces.size(), 0);
}

void MapHeight::read(iStream& in, DWORD fmt)
{
    int n = m_size;
    int index = 0;
    int count = n * n;
    for (int row = 0; row < n; n = m_size, ++row)
    {
        float x = (float)row * 2.0f;
        for (int col = 0; col < m_size; ++col)
        {
            float z = (float)col * 2.0f;
            short height = in.readShort();
            GridPoint& p = m_grid[index++];
            p.x = x;
            p.z = z;
            p.y = (float)height * 0.015f - 200.0f;
        }
    }
    in.read(m_texIndex, count);
}

void MapHeight::write(oStream& out, DWORD fmt) const
{
    int n = m_size;
    int count = n * n;
    if (m_grid)
    {
        for (int row = 0; row < m_size; ++row)
            for (int col = 0; col < m_size; ++col)
                out << (unsigned short)(int)((m_grid[col + row * m_size].y + 200.0f) / 0.015f);
    }
    if (fmt != 'raw ' && m_texIndex)
        out.put(m_texIndex, count);
}

void MapHeight::computeBounds()
{
    GridPoint* grid = m_grid;
    if (!grid)
        return;
    int n = m_size;
    int count = n * n;
    m_min.x = 0.0f;
    m_min.z = 0.0f;
    m_min.y = FLT_MAX;
    float extent = (float)(n - 1) * 2.0f;
    m_max.x = extent;
    m_max.z = extent;
    m_max.y = -FLT_MAX;
    for (int i = 0; i < count; ++i)
    {
        float y = grid[i].y;
        if (m_min.y > y)
            m_min.y = y;
        if (y > m_max.y)
            m_max.y = y;
    }
    float half = (extent + 0.0f) * 0.5f;
    m_center.y = (m_max.y + m_min.y) * 0.5f;
    m_center.x = half;
    m_center.z = half;
}

bool MapHeight::loadTextures()
{
    if (m_tileTextures)
        return true;

    int count = m_strings.size();
    m_bitmaps = new HGDIOBJ[count];
    memset(m_bitmaps, 0, 4 * count);
    bool result = true;
    String folder;
    folder.append(L"Terrain\\Detail\\", (unsigned)-1);

    for (int i = 0; i < count; ++i)
    {
        String* name = m_strings.get(i);
        const wchar_t* text = name ? name->m_buffer : 0;
        if (text)
        {
            String path = folder + text;
            bool failed = !loadTextureBitmap(path, (HBITMAP*)&m_bitmaps[i]);
            if (failed)
                return false;
        }
    }

    long key = 0x1111;
    for (int i = 0; i < count; ++i, key += 0x1111)
    {
        HGDIOBJ bitmap = m_bitmaps[i];
        if (bitmap)
        {
            GLuint texture = 0;
            if (!createTextureFromBitmap((HBITMAP)bitmap, &texture))
            {
                for (int k = 0; k < m_textures.m_count; ++k)
                    m_textures.clearEntry(&m_textures.m_table[k]);
                m_textures.m_count = 0;
                return false;
            }
            m_textures.set(key, texture);
        }
    }

    int cells = m_size * m_size;
    m_tileTextures = new GLuint[cells];
    memset(m_tileTextures, 0, 4 * cells);
    for (int col = 0; col < m_size - 1; col += 8)
    {
        for (int row = 0; row < m_size - 1; row += 8)
        {
            int n = m_size;
            BYTE* p = m_texIndex + row * n + col;
            long corners = 0;
            if (p[0] != 255)
                corners = p[0] << 12;
            BYTE* q = m_texIndex + (row + 8) * n + col;
            if (q[0] != 255)
                corners += q[0] << 8;
            if (q[8] != 255)
                corners += q[8] << 4;
            if (p[8] != 255)
                corners += p[8];

            GLuint texture;
            int k = 0;
            for (; k < m_textures.m_count; ++k)
                if (m_textures.m_table[k].key == corners)
                    break;
            if (k >= m_textures.m_count || k == -1)
                texture = 0;
            else
                texture = m_textures.m_table[k].value;

            if (!texture)
            {
                HBITMAP blended = blendCorners((HBITMAP*)m_bitmaps, corners);
                if (blended)
                {
                    createTextureFromBitmap(blended, &texture);
                    DeleteObject(blended);
                }
                m_textures.set(corners, texture);
            }
            m_tileTextures[col + row * m_size] = texture;
        }
    }
    m_texture = (GLuint)-1;
    return result;
}

bool MapHeight::loadMiniMap()
{
    if (m_tileTextures)
        return true;
    String path;
    path.append(L"Interface\\MiniMap\\MiniMap_", (unsigned)-1);
    path << (unsigned long)m_mapId << ".tga";
    HGDIOBJ bitmap = 0;
    bool ok;
    if (loadTextureBitmap(path, (HBITMAP*)&bitmap))
    {
        GLuint texture = 0;
        ok = createTextureFromBitmap((HBITMAP)bitmap, &texture);
        DeleteObject(bitmap);
        if (ok)
        {
            m_texture = texture;
            m_drawMode = 2;
        }
        else
        {
            m_drawMode = 0;
        }
    }
    else
    {
        ok = false;
    }
    return ok;
}

void MapHeight::drawFaces()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glBegin(GL_QUADS);
    for (int row = 0; row < m_size - 1; ++row)
    {
        for (int col = 0; col < m_size - 1; ++col)
        {
            int n = m_size;
            const GridPoint* a = &m_grid[col + row * n];
            const GridPoint* b = &m_grid[col + (row + 1) * n];
            glVertex3f(a[0].x, a[0].y, a[0].z);
            glVertex3f(a[1].x, a[1].y, a[1].z);
            glVertex3f(b[1].x, b[1].y, b[1].z);
            glVertex3f(b[0].x, b[0].y, b[0].z);
        }
    }
    glEnd();
    if (m_followEye)
    {
        fPoint* eye = m_eye;
        if (eye)
        {
            int n = m_size;
            int row = (int)(eye->x * 0.5f);
            if (row < 0)
                row = 0;
            int col = (int)(eye->z * 0.5f);
            if (row >= n)
                row = n - 1;
            if (col < 0)
                col = 0;
            if (col >= n)
                col = n - 1;
            eye->y = m_grid[col + row * n].y + 8.0f;
        }
    }
}

GLenum MapHeight::drawTextured()
{
    if (!m_texture)
    {
        drawFaces();
        return 0;
    }
    glBegin(GL_QUADS);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    int last = m_size - 1;
    float scale = 1.0f / ((float)last * 2.0f);
    for (int row = 0; row < last; last = m_size - 1, ++row)
    {
        for (int col = 0; col < m_size - 1; ++col)
        {
            int n = m_size;
            const GridPoint* a = &m_grid[col + row * n];
            const GridPoint* b = &m_grid[col + (row + 1) * n];
            glTexCoord2f(a[0].x * scale, -(a[0].z * scale));
            glVertex3f(a[0].x, a[0].y, a[0].z);
            glTexCoord2f(scale * a[1].x, -(a[1].z * scale));
            glVertex3f(a[1].x, a[1].y, a[1].z);
            glTexCoord2f(b[1].x * scale, -(b[1].z * scale));
            glVertex3f(b[1].x, b[1].y, b[1].z);
            glTexCoord2f(b[0].x * scale, -(scale * b[0].z));
            glVertex3f(b[0].x, b[0].y, b[0].z);
        }
    }
    glEnd();
    if (m_followEye)
    {
        fPoint* eye = m_eye;
        if (eye)
        {
            int row = (int)(fabsf(eye->x + 8.0f) * 0.5f);
            float colf = fabsf(m_eye->z + 8.0f) * 0.5f;
            if (row < 0)
                row = 0;
            int n = m_size;
            int col = (int)colf;
            if (row >= n)
                row = n - 1;
            if (col < 0)
                col = 0;
            if (col >= n)
                col = n - 1;
            m_eye->y = m_grid[col + row * n].y + 16.0f;
        }
    }
    return 0;
}
