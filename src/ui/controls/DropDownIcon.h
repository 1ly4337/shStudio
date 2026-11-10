#pragma once
#include <windows.h>
#include "DropDown.h"

class GDC;
class ImageStrip;

class DropDownIcon : public DropDown
{
public:
    explicit DropDownIcon(ImageStrip* icons) : m_icons(icons) {}

    LPCWSTR getClassName() override;

    int getItemCount() override;
    void drawItem(GDC* dc, int index, RECT* rc) override;

public:
    ImageStrip* m_icons;
};
static_assert(sizeof(DropDownIcon) == 0x54, "DropDownIcon size");
