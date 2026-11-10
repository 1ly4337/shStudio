#pragma once
#include "ItemEditor.h"

class PetEditor : public ItemEditor
{
public:
    PetEditor(Window* owner, Data::Items* items, Data::Item* item) : ItemEditor(owner, items, item) {}

    bool onCreate() override;
    bool createEditor() override;

};
static_assert(sizeof(PetEditor) == 0x4C8, "PetEditor size");
