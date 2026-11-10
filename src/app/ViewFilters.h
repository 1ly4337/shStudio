#pragma once
#include <windows.h>
#include <shobjidl.h>

extern COMDLG_FILTERSPEC g_textFileFilter[1];
extern COMDLG_FILTERSPEC g_bmpFileFilter[1];
extern COMDLG_FILTERSPEC g_ddsFileFilters[2];
extern COMDLG_FILTERSPEC g_tgaFileFilters[2];

HBITMAP loadBitmapFromMemory(const void* data, size_t size);
