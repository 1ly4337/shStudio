#include "BitmapsID.h"
#include "ContentProvider.h"
#include "String.hpp"
#include "Bytes.h"
#include "ptrArray.h"
#include "ida_compat.h"
#include "_imports_p09.h"
#include "_imports_g2.h"
#include <cstdlib>
#include "FileTree.h"

BitmapsID::Entry* BitmapsID::clearEntry(Entry* e)
{
    BOOL result = DeleteObject(e->value);
    e->key = 0;
    e->value = 0;
    return (Entry*)(INT_PTR)result;
}

HRESULT loadBitmapsID(FolderTree* folder, BitmapsID* ids)
{
    ContentProvider* provider = g_contentProvider;
    if (!g_contentProvider)
        return E_FAIL;
    ptrArray<FileTree*>& files = FIELD(ptrArray<FileTree*>, folder, 0x64);
    int count = files.size();
    for (int i = 0; i < count; ++i)
    {
        FileTree* file = files.get(i);
        if (!file)
            continue;
        HGDIOBJ bitmap = 0;
        Bytes data;
        if (!provider->readFile(file, &data, false) && !loadTGA(data, &bitmap))
        {
            String name(FIELD(String, file, 0x30));
            name.removeExtension();
            wchar_t* text = name.m_buffer;
            int id = _wtoi(text);
            ids->set(id, (HBITMAP)bitmap);
        }
    }
    return S_OK;
}
