#include "ItemList.h"
#include "TabCtrl.h"
#include "HTablePtr.h"

template class HTable<long, ItemList*>;
template class HTablePtr<long, ItemList*>;
template class TabCtrl<ItemList*>;
