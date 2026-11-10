#pragma once
#include "Container.h"

class IniFile;

class OptionPanel : public Container
{
public:
    OptionPanel() {}
    virtual ~OptionPanel() {}

    virtual void load(IniFile* prefs) = 0;
    virtual void save(IniFile* prefs) = 0;
    virtual bool validate() { return true; }
};
static_assert(sizeof(OptionPanel) == 0x68, "OptionPanel size");
