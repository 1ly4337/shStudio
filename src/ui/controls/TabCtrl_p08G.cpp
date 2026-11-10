#include "TabCtrl.h"
#include "HTablePtr.h"
#include "ptrArray.h"
#include "NpcSkillList.h"
#include "MobsSkill.h"

template class TabCtrl<NpcSkillList*>;
template class HTablePtr<long, NpcSkillList*>;
template class ptrArray<MobsSkill*>;
