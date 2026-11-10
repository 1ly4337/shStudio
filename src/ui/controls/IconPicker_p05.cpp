#include "p05_common.h"
#include "IconPicker.h"

IconPicker::~IconPicker()
{
}

void IconPicker::onFocus(bool )
{
    invalidate(false);
}
