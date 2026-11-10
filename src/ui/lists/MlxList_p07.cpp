#include "MlxList.h"
#include "ModelMLX.h"
#include "ModelsMLX.h"
#include "StudioFrame.h"
#include "Clipboard.h"
#include "Bytes.h"
#include "String.hpp"
#include "mlx_g1.h"
#include "_imports_p07.h"

static inline String* p07_tmpString(String&& s) { return &s; }
#include <shlwapi.h>

#pragma warning(disable : 4066 4498)

#define MLXLIST_MODELS(l) FIELD(ModelsMLX*, (l), 0xF8)
#define MLXLIST_FOLDER(l) FIELD(String*, (l), 0xFC)

bool MlxList::onCreate()
{
    bool result = ListCtrlOD::onCreate();
    if (result)
    {
        for (int i = 0; i < 6; ++i)
        {
            int format = 0;
            if (!i)
                format = 16;
            addColumn(g_mlxColumnTitles[i], g_mlxColumnWidths[i], format, g_mlxColumnIds[i]);
        }
        return true;
    }
    return result;
}

bool MlxList::onCommand(int code, LPARAM ctrl, int id)
{
    if ((code | 1) == 1 && !ctrl && (id == 58024 || id == 58025))
        return true;
    return ListCtrlOD::onCommand(code, ctrl, id);
}

bool MlxList::moveItemUp(int index)
{
    bool result = MLXLIST_MODELS(this)->swap(index, index - 1);
    if (result)
    {
        ModelMLX* prev = MLXLIST_MODELS(this)->get(index - 1);
        ModelMLX* cur = MLXLIST_MODELS(this)->get(index);
        int id = MLX_ID(cur);
        MLX_ID(cur) = MLX_ID(prev);
        MLX_ID(prev) = id;
        onChanged();
        return true;
    }
    return result;
}

bool MlxList::moveItemDown(int index)
{
    bool result = MLXLIST_MODELS(this)->swap(index, index + 1);
    if (result)
    {
        ModelMLX* next = MLXLIST_MODELS(this)->get(index + 1);
        ModelMLX* cur = MLXLIST_MODELS(this)->get(index);
        int id = MLX_ID(cur);
        MLX_ID(cur) = MLX_ID(next);
        MLX_ID(next) = id;
        onChanged();
        return true;
    }
    return result;
}

bool MlxList::onCopied(Node* node)
{
    ModelMLX* mlx = dynamic_cast<ModelMLX*>(node);
    if (!mlx)
        return false;

    String ddsDir = *MLXLIST_FOLDER(this) + L"DDS\\";
    String tdcDir = *MLXLIST_FOLDER(this) + L"3DC\\";
    oStream out(false, 1200);
    Bytes file;
    bool result;

    for (int i = 0; i < 4; ++i)
    {
        file.wipe();
        bool failed = g_studioFrame->loadFromFile(&file, p07_tmpString(ddsDir + MLX_DDS(mlx, i)), false) != 0;
        if (failed)
        {
            result = false;
            return result;
        }
        out << (unsigned long)file.m_size;
        out.put(file.m_data + file.m_pos, file.m_size - file.m_pos);

        file.wipe();
        failed = g_studioFrame->loadFromFile(&file, p07_tmpString(tdcDir + MLX_3DC(mlx, i)), false) != 0;
        if (failed)
        {
            result = false;
            return result;
        }
        out << (unsigned long)file.m_size;
        out.put(file.m_data + file.m_pos, file.m_size - file.m_pos);
    }

    Clipboard::setData(0x22D, out, false);
    result = true;
    return result;
}

void MlxList::onPasted(Node* node)
{
    Bytes clip;
    Bytes file;
    ModelMLX* mlx = dynamic_cast<ModelMLX*>(node);
    if (mlx
        && !g_studioFrame->isArchiveReadOnly()
        && Clipboard::getData(0x22D, clip))
    {
        String ddsDir = *MLXLIST_FOLDER(this) + L"DDS\\";
        String tdcDir = *MLXLIST_FOLDER(this) + L"3DC\\";
        for (int i = 0; i < 4; ++i)
        {
            int size = (int)clip.readInt();
            clip.readBytes(file, size);
            bool missing = !PathFileExistsW((ddsDir + MLX_DDS(mlx, i)).c_str());
            if (missing)
                g_studioFrame->saveToFile(&file, p07_tmpString(ddsDir + MLX_DDS(mlx, i)), false);

            size = (int)clip.readInt();
            clip.readBytes(file, size);
            missing = !PathFileExistsW((tdcDir + MLX_3DC(mlx, i)).c_str());
            if (missing)
                g_studioFrame->saveToFile(&file, p07_tmpString(tdcDir + MLX_3DC(mlx, i)), false);
        }
    }
}
