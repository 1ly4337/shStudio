#include "p02_common.h"
#include "ItemModels.h"
#include "ItemModel.h"
#include "Strings.h"

void ItemModels::read(iStream& in, DWORD fmt)
{
    deleteAll();
    try
    {
        m_version = 0x20000000;
        in.read(&m_version, 3);
        m_version = (m_version >> 24) | ((m_version >> 8) & 0xFF00) | ((m_version << 8) & 0xFF0000) | (m_version << 24);
        if (m_version != 'ITM ' && m_version != 'IT2 ')
            throw;
        Strings vertices(32);
        vertices.m_allowDuplicates = true;
        Strings textures(32);
        textures.m_allowDuplicates = true;
        String name;
        int n = in.readInt();
        for (int i = 0; i < n; ++i)
        {
            name.read(in, 4, (UINT)-1);
            vertices.add((long)i, name.m_buffer);
        }
        n = in.peekInt();
        in.skip(4);
        for (int i = 0; i < n; ++i)
        {
            name.read(in, 4, (UINT)-1);
            textures.add((long)i, name.m_buffer);
        }
        n = in.peekInt();
        in.skip(4);
        for (int k = 0; k < n; ++k)
        {
            ItemModel* model = new ItemModel(k, m_version, 0);
            model->read(in, fmt);
            model->m_texture = *textures.findById(model->m_textureIndex);
            model->m_vertex = *vertices.findById(model->m_vertexIndex);
            add(model);
        }
    }
    catch (...)
    {
        deleteAll();
        throw;
    }
}

void ItemModels::write(oStream& out, DWORD fmt) const
{
    out.put("ITM", 3);
    Strings vertices(32);
    vertices.m_allowDuplicates = true;
    Strings textures(32);
    textures.m_allowDuplicates = true;
    for (int i = 0; i < m_count; ++i)
        vertices.add(m_items[i]->m_vertex.m_buffer);
    for (int i = 0; i < m_count; ++i)
        textures.add(m_items[i]->m_texture.m_buffer);
    int n = vertices.m_count;
    out << (unsigned long)n;
    for (int k = 0; k < n; ++k)
        vertices.at(k)->write(out, 4, (UINT)-1);
    n = textures.m_count;
    out << (unsigned long)n;
    for (int k = 0; k < n; ++k)
        textures.at(k)->write(out, 4, (UINT)-1);
    out << (unsigned long)m_count;
    for (int i = 0; i < m_count; ++i)
    {
        ItemModel* m = m_items[i];
        m->m_vertexIndex = vertices.indexOf(m->m_vertex.m_buffer);
        m->m_textureIndex = textures.indexOf(m->m_texture.m_buffer);
        m->write(out, fmt);
    }
}
