#include "p05_common.h"
#include "CharActions.h"

CharActions::CharActions()
    : ptrArray<CharAction*>(256), Serializable()
{
}

CharActions::~CharActions()
{
}
