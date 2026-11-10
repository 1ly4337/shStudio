#pragma once
#include <windows.h>
#include <cstdint>

typedef uint8_t  _BYTE;
typedef uint16_t _WORD;
typedef uint32_t _DWORD;
typedef uint64_t _QWORD;

#define LOBYTE_(x)  (*((uint8_t*)&(x)))
#define BYTEn(x, n) (*((uint8_t*)&(x) + (n)))
#define WORDn(x, n) (*((uint16_t*)&(x) + (n)))
#define BYTE1(x) BYTEn(x, 1)
#define BYTE2(x) BYTEn(x, 2)
#define BYTE3(x) BYTEn(x, 3)
#define __PAIR64__(high, low) (((uint64_t)(uint32_t)(high) << 32) | (uint32_t)(low))

template <class T> inline T __ROL4__(T v, int n) { return (T)(((uint32_t)v << n) | ((uint32_t)v >> (32 - n))); }
template <class T> inline T __ROR4__(T v, int n) { return (T)(((uint32_t)v >> n) | ((uint32_t)v << (32 - n))); }

#define FIELD(T, base, off) (*(T*)((uint8_t*)(base) + (off)))
