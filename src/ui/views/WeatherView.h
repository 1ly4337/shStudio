#pragma once
#include "StringView.h"
#include "Weather.h"

class WeatherView : public StringView<Weather> {
public:
    explicit WeatherView(FileTree* node) : StringView<Weather>(node) {}
};
static_assert(sizeof(WeatherView) == 0x1FC, "WeatherView size");
