#pragma once
#include <windows.h>
#include "ida_compat.h"

const wchar_t* mlxRaceDescription(const wchar_t* code);
int mlxRaceIndex(const wchar_t* code);

extern const wchar_t* const g_mlxPartNames[4];
extern const wchar_t* const g_mlxColumnTitles[6];
extern const int g_mlxColumnWidths[6];
extern const DWORD g_mlxColumnIds[6];

#define MLX_ID(m)       FIELD(int, (m), 0x0C)
#define MLX_NAME(m)     FIELD(String, (m), 0x10)
#define MLX_DDS(m, i)   FIELD(String, (m), 0x30 + 0x20 * (i))
#define MLX_3DC(m, i)   FIELD(String, (m), 0xB0 + 0x20 * (i))
#define MLX_ALPHA(m, i) FIELD(int, (m), 0x130 + 4 * (i))

#define MLT_ID(m)       FIELD(int, (m), 0x0C)
#define MLT_ALPHA(m)    FIELD(int, (m), 0x14)
#define MLT_DDS(m)      FIELD(String, (m), 0x18)
#define MLT_3DC(m)      FIELD(String, (m), 0x38)

#define MV_CHILD(v)        ((v)->m_child)
#define MV_VIEW3D(v)       ((v)->m_view3D)
#define MV_TARGET(v)       FIELD(fPoint, &(v)->m_view3D, 0x374)
#define MV_SELECTED(v)     ((v)->m_selected)
#define MV_TARGET_Y(v)     ((v)->m_targetY)
#define MV_SHOWMODELS(v)   ((v)->m_showModels)

#define MLXV_MODELS(v)     FIELD(ModelsMLX*, (v), 0x448)
#define MLXV_CODE(v)       ((wchar_t*)((BYTE*)(v) + 0x44C))
#define MLXV_RACE(v)       FIELD(int, (v), 0x458)
#define MLXV_LIST(v)       FIELD(MlxList, (v), 0x45C)
#define MLXV_STATIC(v, i)  FIELD(HWND, (v), 0x55C + 4 * (i))
#define MLXV_GEARS(v, i)   FIELD(ListGears, (v), 0x574 + 0x120 * (i))
#define MLXV_NAME(v)       FIELD(EditText, (v), 0x9F4)
#define MLXV_3DC(v, i)     FIELD(EditText, (v), 0xA00 + 12 * (i))
#define MLXV_DDS(v, i)     FIELD(EditText, (v), 0xA30 + 12 * (i))
#define MLXV_ALPHA(v, i)   FIELD(ComboBox, (v), 0xA60 + 40 * (i))
#define MLXV_BTN_UPDATE(v) FIELD(Button, (v), 0xB00)
#define MLXV_BTN_APPEND(v) FIELD(Button, (v), 0xB0C)
#define MLXV_BTN_RENAME(v) FIELD(Button, (v), 0xB18)
