#include "p02_common.h"
#include "DualLayerModels.h"
#include "DualLayerModel.h"

void DualLayerModels::read(iStream& in, DWORD fmt)
{
    if (m_buffer1)
    {
        delete[] m_buffer1;
        m_buffer1 = 0;
    }
    deleteAll();
    m_format = 0x600;
    if (fmt == 'flnt')
    {
        m_buffer1 = (wchar_t*)in.readAlloc(128);
        if (in.readDword() != 7)
            throw L"Invalid or not supported DB format";
        m_format |= 0x1000;
        Serializable::checkSignature(in, L"id");
        Serializable::checkSignature(in, L"top");
        Serializable::checkSignature(in, L"hand");
        Serializable::checkSignature(in, L"bottom");
        Serializable::checkSignature(in, L"shoe");
        Serializable::checkSignature(in, L"empty");
        Serializable::checkSignature(in, L"helmet");
    }
    else if (fmt != 'file')
    {
        throw L"Unsupported client";
    }
    int n = in.readInt();
    try
    {
        for (int i = 0; i < n; ++i)
        {
            DualLayerModel* m = new DualLayerModel(m_format, 0);
            add(m);
            m->read(in, fmt);
        }
    }
    catch (...)
    {
        deleteAll();
        throw;
    }
}

void DualLayerModels::write(oStream& out, DWORD fmt) const
{
    if (fmt == 'flnt')
    {
        out.put(m_buffer1, 128);
        out << (unsigned long)7;
        out << L"id";
        out << L"top";
        out << L"hand";
        out << L"bottom";
        out << L"shoe";
        out << L"empty";
        out << L"helmet";
    }
    out << (unsigned long)m_count;
    for (int i = 0; i < m_count; ++i)
        m_items[i]->write(out, fmt);
}

bool DualLayerModels::toText(String& out, DWORD fmt) const
{
    bool ok = true;
    for (int i = 0; i < m_count && ok; ++i)
        ok = m_items[i]->toText(out, fmt);
    return ok;
}
