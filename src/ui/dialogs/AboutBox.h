#pragma once
#include "Modal.h"

class AboutBox : public Modal {
public:
    AboutBox() : Modal() {}
    bool create(HWND parent);
    bool onCreate() override;
};
