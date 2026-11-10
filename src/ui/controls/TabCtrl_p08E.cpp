#include "TabCtrl.h"
#include "HTablePtr.h"
#include "ptrArray.h"
#include "NpcList.h"
#include "QuestDetails.h"

template class TabCtrl<NpcList*>;
template class HTablePtr<long, NpcList*>;
template class ptrArray<QuestDetails*>;
