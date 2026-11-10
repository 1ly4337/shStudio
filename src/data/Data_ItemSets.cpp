#include "Data_ItemSets.h"
#include "_imports_p07.h"

namespace Data {

ItemSets::ItemSets()
    : ptrArray<ItemSet*>(256)
    , Serializable()
{
}

ItemSets::~ItemSets()
{
}

}
