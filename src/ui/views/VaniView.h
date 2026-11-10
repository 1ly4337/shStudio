#pragma once
#include "ThreeDxView.h"

class VaniView : public ThreeDxView {
public:
    explicit VaniView(FileTree* node) : ThreeDxView(node, 'kVAn') {}
    ~VaniView() override {}
    LPCWSTR getClassName() override;
    bool onCreate() override;
};
static_assert(sizeof(VaniView) == 0x258, "VaniView size");
