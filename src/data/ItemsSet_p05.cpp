#include "p05_common.h"
#include "ItemsSet.h"

namespace Data {

ItemsSet::ItemsSet()
    : ptrArray<Data::Item*>(256), Node()
{
}

}
