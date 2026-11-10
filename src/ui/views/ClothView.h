#pragma once
#include "StringView.h"
#include "ClothTextures.h"

class ClothView : public StringView<ClothTextures> {
public:
    explicit ClothView(FileTree* node) : StringView<ClothTextures>(node) {}
};
static_assert(sizeof(ClothView) == 0x1F4, "ClothView size");
