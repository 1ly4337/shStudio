#pragma once
#include "SettingsDial.h"

class Options : public SettingsDial {
public:
    explicit Options(void* owner) : SettingsDial(owner) {}
    ~Options() override;
    bool createPanels() override;
};
