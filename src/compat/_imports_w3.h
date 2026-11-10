#pragma once
#include <windows.h>

class Bytes;

extern DWORD loadDDS(Bytes& data, HGDIOBJ* outBitmap);
extern DWORD loadTGA(Bytes& data, HGDIOBJ* outBitmap);
extern bool byte_59FC6A;
