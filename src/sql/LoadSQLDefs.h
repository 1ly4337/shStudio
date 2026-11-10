#pragma once
#include "Thread.h"

class StudioFrame;

class LoadSQLDefs : public Thread {
public:
    explicit LoadSQLDefs(StudioFrame* frame) : Thread(), m_frame(frame) {}
    void run() override;

    StudioFrame* m_frame;
};
static_assert(sizeof(LoadSQLDefs) == 0x1C, "LoadSQLDefs size");
