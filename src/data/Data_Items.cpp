#include "Data_Items.h"
#include "_imports_p07.h"
#if __has_include("Data_Item.h")
#include "Data_Item.h"
#endif

namespace Data {

Items::Items()
    : Serializable()
    , ptrArray<Item*>(256)
{
    m_unk2C = 0;
    m_value30 = 1280;
}

Items::~Items()
{
}

void Items::clear()
{
    deleteAll();
}

}
