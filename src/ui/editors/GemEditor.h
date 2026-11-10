#pragma once
#include "ItemEditor.h"

class GemEditor : public ItemEditor
{
public:
    GemEditor(Window* owner, Data::Items* items, Data::Item* item) : ItemEditor(owner, items, item) {}
    virtual ~GemEditor() {}
};
static_assert(sizeof(GemEditor) == 0x4C8, "GemEditor size");
