#include "SEED.h"
#include "AES.h"
#include "aStream.h"
#include <cstring>

const char g_dataFileSignature[41] = "0001CBCEBC5B2784D3FC9A2A9DB84D1C3FEB6E99";

namespace {

inline DWORD seedG(DWORD x)
{
    return g_seedSS0[x & 0xFF] ^ g_seedSS1[(x >> 8) & 0xFF] ^ g_seedSS2[(x >> 16) & 0xFF] ^ g_seedSS3[x >> 24];
}

inline DWORD seedSwap(DWORD x)
{
    return ((x << 8 | x >> 24) & 0x00FF00FF) | ((x << 24 | x >> 8) & 0xFF00FF00);
}

inline void seedRound(DWORD& l0, DWORD& l1, DWORD r0, DWORD r1, const DWORD* k)
{
    DWORD t0 = r0 ^ k[0];
    DWORD t1 = r1 ^ k[1];
    t1 ^= t0;
    t1 = seedG(t1);
    t0 += t1;
    t0 = seedG(t0);
    t1 += t0;
    t1 = seedG(t1);
    t0 += t1;
    l0 ^= t0;
    l1 ^= t1;
}

}

SEED::SEED() : m_ready(false)
{
    memset(m_keys, 0, sizeof(m_keys));
}

SEED::~SEED()
{
    memset(m_keys, 0, sizeof(m_keys));
}

void SEED::setDataFileKey()
{
    for (int i = 0; i < 32; ++i)
        m_keys[i] = g_dataFileRoundKeys[i] ^ 0x55AA55AA;
    m_ready = true;
}

void SEED::transformBlock(const BYTE* in, BYTE* out, bool encrypt) const
{
    DWORD w[4];
    memcpy(w, in, 16);
    DWORD l0 = seedSwap(w[0]), l1 = seedSwap(w[1]), r0 = seedSwap(w[2]), r1 = seedSwap(w[3]);
    if (encrypt) {
        for (int round = 0; round < 32; round += 4) {
            seedRound(l0, l1, r0, r1, m_keys + round);
            seedRound(r0, r1, l0, l1, m_keys + round + 2);
        }
    } else {
        for (int round = 30; round >= 0; round -= 4) {
            seedRound(l0, l1, r0, r1, m_keys + round);
            seedRound(r0, r1, l0, l1, m_keys + round - 2);
        }
    }
    w[0] = seedSwap(r0);
    w[1] = seedSwap(r1);
    w[2] = seedSwap(l0);
    w[3] = seedSwap(l1);
    memcpy(out, w, 16);
}

int SEED::encrypt(const BYTE* in, int size, BYTE* out) const
{
    if (!size)
        return size;
    if (!m_ready)
        throw crypto_exception();
    if (size & 0xF)
        throw crypto_exception();
    for (int i = 0; i < size; i += 16)
        transformBlock(in + i, out + i, true);
    return size;
}

int SEED::decrypt(const BYTE* in, int size, BYTE* out) const
{
    if (!size)
        return size;
    if (!m_ready)
        throw crypto_exception();
    if (size & 0xF)
        throw crypto_exception();
    for (int i = 0; i < size; i += 16)
        transformBlock(in + i, out + i, false);
    return size;
}

bool __fastcall sub_418C00(aStream* data, void* flag, void* key)
{
    if (flag)
        *(char*)flag = 0;
    unsigned size = (unsigned)data->m_size;
    if (size < 0x40 || (size & 0xF) != 0)
        return true;
    DWORD local[16];
    memset(local, 0, sizeof(local));
    DWORD* header = key ? (DWORD*)key : local;
    memcpy(header, data->m_data + data->m_pos, 0x40);
    unsigned encryptedSize = size - 0x40;
    if (memcmp(header, g_dataFileSignature, 16) != 0)
        return true;
    if (flag)
        *(char*)flag = 1;
    unsigned plainSize = header[10] ? header[11] : header[12];
    if (plainSize < encryptedSize && plainSize > encryptedSize - 16)
        plainSize = encryptedSize;
    try {
        BYTE* buffer = (BYTE*)::operator new[](encryptedSize ? encryptedSize : 1);
        struct Holder { BYTE* p; ~Holder() { ::operator delete[](p); } } hold = { buffer };
        memcpy(buffer, data->m_data + data->m_pos + 0x40, encryptedSize);
        SEED seed;
        seed.setDataFileKey();
        if (seed.decrypt(buffer, (int)encryptedSize, buffer) != (int)encryptedSize)
            throw 0;
        return data->assign(buffer, (int)plainSize);
    } catch (...) {
        return false;
    }
}

bool __fastcall sub_418E60(aStream* data, void* headerBytes)
{
    BYTE* header = (BYTE*)headerBytes;
    DWORD* words = (DWORD*)headerBytes;
    int size = data->m_size;
    bool crcAtSecond = words[10] == 0;
    data->m_pos = 0;
    if (!size)
        return false;
    BYTE any = 0;
    for (int i = 0; i < 40; ++i)
        any |= header[i];
    if (!any)
        memcpy(header, g_dataFileSignature, 40);
    if (crcAtSecond)
        words[12] = (DWORD)size;
    else
        words[11] = (DWORD)size;
    int padded = size;
    if (size % 16)
        padded = size - size % 16 + 16;
    DWORD crc = 0xFFFFFFFF;
    const BYTE* source = data->m_data + data->m_pos;
    for (int i = 0; i < size; ++i)
        crc = g_dataFileCrc[(BYTE)crc ^ source[i]] ^ (crc >> 8);
    crc = ~crc;
    if (crcAtSecond)
        words[11] = crc;
    else
        words[10] = crc;
    BYTE* buffer = (BYTE*)::operator new[](padded);
    memset(buffer + padded - 16, 0, 16);
    memcpy(buffer, source, size);
    {
        SEED seed;
        seed.setDataFileKey();
        if (seed.encrypt(buffer, padded, buffer) != padded) {
            ::operator delete[](buffer);
            throw 0;
        }
    }
    if (padded >= 1) {
        if (data->m_capacity >= padded || data->grow(padded - data->m_capacity)) {
            memset(data->m_data, 0, data->m_capacity);
            memcpy(data->m_data, buffer, padded);
            data->m_size = padded;
            data->m_pos = 0;
        }
    }
    ::operator delete[](buffer);
    return true;
}
