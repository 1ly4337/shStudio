#include "ContentFiles.h"
#include <shlwapi.h>
#include <cstdio>
#include <cstdlib>
#include "String.hpp"
#include "Exception.h"
#include "Bytes.h"
#include "FolderRoot.h"
#include "ArchiveTree_g3.h"
#include "_imports_p09.h"
#include "_imports_g3.h"

int loadDataFile(FileTree* file, iStream* out, bool decrypt)
{
    aStream& target = *out;
    target.wipe();
    Bytes data;
    int result = data.aStream::load(FileTree_path(file).m_buffer);
    if (!result)
    {
        aStream& source = data;
        if (!decrypt || sub_418C00(&source, 0, FileTree_key(file)))
            target.assign(source.m_data, source.m_size);
        else
            result = E_FAIL;
    }
    return result;
}

int encryptDataFile(aStream* in, BYTE* key, Bytes& out)
{
    aStream& source = *in;
    Bytes copy(source.m_data, source.m_size);
    int result;
    if (sub_418E60(&static_cast<aStream&>(copy), key))
    {
        out.wipe();
        static_cast<oStream&>(out).put(key, 64);
        out.Bytes::append(copy);
        result = 0;
    }
    else
    {
        result = E_FAIL;
    }
    return result;
}

int getContentFileType(wchar_t* fileName)
{
    wchar_t* extension = PathFindExtensionW(fileName);
    if (!extension)
        return -1;
    int type;
    String ext(extension + 1);
    ext.toLowerInPlace();
    if (ext.equalsNoCase(L"bin"))
        type = -1;
    else if (ext.equalsNoCase(L"txt") || ext.equalsNoCase(L"ctl") || ext.equalsNoCase(L"dat")
             || ext.equalsNoCase(L"wtr") || ext.equalsNoCase(L"lua"))
        type = 2;
    else if (ext.equalsNoCase(L"wav") || ext.equalsNoCase(L"wave"))
        type = 3;
    else if (ext.equalsNoCase(L"alt") || ext.equalsNoCase(L"mlt") || ext.equalsNoCase(L"mlx")
             || ext.equalsNoCase(L"mon") || ext.equalsNoCase(L"itm"))
        type = 4;
    else if (ext.equalsNoCase(L"cfg"))
        type = 5;
    else if (ext.equalsNoCase(L"bmp") || ext.equalsNoCase(L"dds") || ext.equalsNoCase(L"jpg")
             || ext.equalsNoCase(L"tga"))
        type = 6;
    else if (ext.equalsNoCase(L"3de") || ext.equalsNoCase(L"3dc") || ext.equalsNoCase(L"3do"))
        type = 7;
    else if (ext.equalsNoCase(L"dg") || ext.equalsNoCase(L"smod") || ext.equalsNoCase(L"vani"))
        type = 8;
    else if (ext.equalsNoCase(L"wld"))
    {
        FILE* stream = 0;
        if (_wfopen_s(&stream, fileName, L"r"))
        {
            type = 9;
        }
        else
        {
            unsigned int magic = 0;
            fread(&magic, 4, 1, stream);
            fclose(stream);
            type = (_byteswap_ulong(magic) != 0x44554E00) ? 9 : 10;
        }
    }
    else if (ext.equalsNoCase(L"sdata"))
        type = 11;
    else if (ext.equalsNoCase(L"ani") || ext.equalsNoCase(L"ini") || ext.equalsNoCase(L"seff")
             || ext.equalsNoCase(L"zon"))
        type = 12;
    else if (ext.equalsNoCase(L"eft"))
        type = 13;
    else if (ext.equalsNoCase(L"cur") || ext.equalsNoCase(L"dat"))
        type = 14;
    else if (ext.equalsNoCase(L"svmap"))
        type = 20;
    else
        type = 15;
    return type;
}

int addArchiveFile(FolderRoot* root, String& dir, wchar_t* name, int codePage)
{
    int result;
    {
        String full = dir + name;
        result = (int)(INT_PTR)root->findFile(full.m_buffer);
    }
    if (!result)
    {
        FolderTree* folder = (FolderTree*)root->findFolderPath(dir.m_buffer);
        result = (int)(INT_PTR)folder;
        if (folder)
        {
            FileTree* file = new FileTree(folder, name, codePage);
            FileTree_path(file) = dir + name;
            return FolderTree_files(folder).add(file);
        }
    }
    return result;
}
