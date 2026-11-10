#include "p04_common.h"
#include "TransformWeaponModels.h"
#include "TransformWeaponModel.h"
#include "iStream.h"
#include "oStream.h"

void TransformWeaponModels::read(iStream& in, DWORD fmt)
{
    if (m_buffer1)
    {
        delete[] m_buffer1;
        m_buffer1 = 0;
    }
    deleteAll();
    if (fmt != 'flnt')
        throw (wchar_t*)L"Unsupported client";
    m_buffer1 = (wchar_t*)in.readAlloc(128);
    if (in.readDword() != 3)
        throw (const wchar_t*)L"Invalid or not supported DB format";
    Serializable::checkSignature(in, L"type");
    Serializable::checkSignature(in, L"weapon");
    Serializable::checkSignature(in, L"weapon1");
    int count = (int)in.readInt();
    for (int i = 0; i < count; ++i)
    {
        TransformWeaponModel* model = new TransformWeaponModel();
        add(model);
        model->read(in, 'flnt');
    }
}

void TransformWeaponModels::write(oStream& out, DWORD fmt) const
{
    if (fmt != 'flnt')
        throw (wchar_t*)L"Unsupported client";
    out.put(m_buffer1, 128);
    out << (unsigned long)3;
    out << L"type";
    out << L"weapon";
    out << L"weapon1";
    out << (unsigned long)m_count;
    for (int i = 0; i < m_count; ++i)
        m_items[i]->write(out, 'flnt');
}
