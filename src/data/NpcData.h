#pragma once
#include <windows.h>
#include "NpcPerso.h"
#include "Bytes.h"
#include "QuestsDetails.h"

class iStream;
class oStream;

class NpcData : public NpcPerso
{
public:
    NpcData();
    virtual ~NpcData();

    void clear() override;
    int getCodePage() const override;
    const wchar_t* getFilePrefix() const override;
    void load(iStream& in, int arg2, int arg3) override;
    HRESULT setCodePage(int format) override;
    void read(iStream& in, DWORD fmt) override;
    void write(oStream& out, DWORD fmt) const override;

public:
    int           m_format;
    Bytes         m_bytes;
    QuestsDetails m_quests;
};

inline StandardNPCs* npcTypeList(NpcData* data, unsigned char type)
{
    if ((unsigned char)(type - 1) > 12)
        return 0;
    return &data->m_npcs[type];
}
