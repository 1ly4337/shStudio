#pragma once
#include <windows.h>

#pragma pack(push, 4)
class IChrono
{
public:
    virtual BOOL start() = 0;
    virtual int  stop() = 0;
    virtual int  lap() = 0;
};

class QpcChrono : public IChrono
{
public:
    QpcChrono() { QueryPerformanceFrequency(&m_frequency); }

    BOOL start() override;
    int stop() override;
    int lap() override;

public:
    DWORD         m_unused04;
    LARGE_INTEGER m_start;
    LARGE_INTEGER m_frequency;
};
#pragma pack(pop)
static_assert(sizeof(IChrono) == 0x04, "IChrono size");
static_assert(sizeof(QpcChrono) == 0x18, "QpcChrono size");
static_assert(alignof(QpcChrono) == 4, "QpcChrono must not 8-align its owner (glView)");
