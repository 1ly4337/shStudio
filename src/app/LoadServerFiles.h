#pragma once
#include "Thread.h"

class StudioFrame;

class LoadServerFiles : public Thread {
public:
    explicit LoadServerFiles(StudioFrame* frame) : Thread(), m_frame(frame) {}
    void run() override;

    StudioFrame* m_frame;
};
static_assert(sizeof(LoadServerFiles) == 0x1C, "LoadServerFiles size");
