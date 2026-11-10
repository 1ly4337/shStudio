#include <windows.h>
#include "Monsters.h"
#include "Monster.h"
#include "iStream.h"
#include "_imports_C3.h"

Monsters::Monsters()
    : ptrArray<Monster*>(256)
{
    m_format = 0x500;
    m_extended = false;
}

Monsters::~Monsters()
{
}

void Monsters::clear()
{
    deleteAll();
    m_extended = false;
}

const wchar_t* Monsters::getFilePrefix() const
{
    return L"BinarySData\\DBMonsterText_";
}

void Monsters::load(iStream& in, int arg2, int arg3)
{
    read(in, 'ltxt');
}
