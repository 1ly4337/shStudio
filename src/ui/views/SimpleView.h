#pragma once
#include "Document.h"

class SimpleView : public Document {
public:
    explicit SimpleView(FileTree* data) : Document(data) {}
    ~SimpleView() override {}
    bool onCreate() override;
};
static_assert(sizeof(SimpleView) == 0x58, "SimpleView size");
