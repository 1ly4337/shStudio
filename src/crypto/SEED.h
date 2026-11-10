#pragma once
#include <windows.h>

extern const DWORD g_seedSS0[256];
extern const DWORD g_seedSS1[256];
extern const DWORD g_seedSS2[256];
extern const DWORD g_seedSS3[256];
extern const DWORD g_dataFileCrc[256];
extern const DWORD g_dataFileRoundKeys[32];

class SEED
{
public:
    SEED();
    ~SEED();

    void setDataFileKey();
    void transformBlock(const BYTE* in, BYTE* out, bool encrypt) const;
    int  encrypt(const BYTE* in, int size, BYTE* out) const;
    int  decrypt(const BYTE* in, int size, BYTE* out) const;

    bool  m_ready;
    DWORD m_keys[32];
};

extern const char g_dataFileSignature[41];
