#include "p02_common.h"
#include "ModelsMLT.h"
#include "ModelMLT.h"
#include "Strings.h"

ModelMLT* ModelsMLT::detach(int index)
{
    ModelMLT* removed = ptrArray<ModelMLT*>::detach(index);
    if (!removed)
        return 0;
    int id = removed->m_id;
    for (int i = 0; i < m_count; ++i)
    {
        ModelMLT* m = m_items[i];
        if (m->m_id > id)
            m->m_id = m->m_id - 1;
    }
    return removed;
}

void ModelsMLT::read(iStream& in, DWORD )
{
    clear();
    if (in.m_size - in.m_pos < 3)
        throw ArrayIndexOutOfBoundsException(in.m_size - in.m_pos, 3);
    BYTE* p = in.m_data + in.m_pos;
    WORD signature = *(WORD*)p;
    BYTE version = p[2];
    in.m_pos += 3;
    if (signature != 0x4C4D || version != '2')
        throw;
    try
    {
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
        n = in.readInt();
        for (int i = 0; i < n; ++i)
        {
            name.read(in, 4, (UINT)-1);
            textures.add((long)i, name.m_buffer);
        }
        String empty;
        int id = 0;
        n = in.readInt();
        for (int k = 0; k < n; ++k)
        {
            int vertexIndex = in.readInt();
            int textureIndex = in.readInt();
            int alpha = in.readInt();
            const String* vertex = 0;
            if (vertexIndex >= 0)
            {
                try
                {
                    vertex = vertices.strID(vertexIndex);
                }
                catch (...)
                {
                    vertex = &empty;
                }
            }
            if (!vertex)
                vertex = &empty;
            const String* texture = 0;
            if (textureIndex >= 0)
            {
                try
                {
                    texture = textures.strID(textureIndex);
                }
                catch (...)
                {
                    texture = &empty;
                }
            }
            if (!texture)
                texture = &empty;
            ModelMLT* model = new ModelMLT(id, texture->m_buffer, vertex->m_buffer, alpha, id);
            ++id;
            add(model);
        }
        vertices.deleteAll();
        textures.deleteAll();
    }
    catch (...)
    {
        deleteAll();
        throw;
    }
}

void ModelsMLT::write(oStream& out, DWORD ) const
{
    out.put("ML2", 3);
    Strings vertices(16);
    Strings textures(16);
    try
    {
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
            const ModelMLT* m = m_items[i];
            out << (unsigned long)vertices.indexOf(m->m_vertex.m_buffer);
            out << (unsigned long)textures.indexOf(m->m_texture.m_buffer);
            out << (unsigned long)m->m_alpha;
        }
        vertices.deleteAll();
        textures.deleteAll();
    }
    catch (...)
    {
        textures.deleteAll();
        vertices.m_count = 0;
        vertices.deleteAll();
        throw;
    }
}
