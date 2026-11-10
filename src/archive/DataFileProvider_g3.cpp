#include "DataFileProvider.h"
#include <shlwapi.h>
#include <sys/stat.h>
#include <cstdio>
#include "String.hpp"
#include "Bytes.h"
#include "FolderRoot.h"
#include "BrowseFoldersDialog.h"
#include "ContentFiles.h"
#include "ArchiveTree_g3.h"
#include "Window.h"
#include "_imports_p09.h"
#include "_imports_g3.h"

int DataFileProvider::readFile(FileTree* file, iStream* out, bool decrypt)
{
    return loadDataFile(file, out, decrypt);
}

int DataFileProvider::readPath(const wchar_t* path, iStream* out)
{
    if (!path)
        return 161;
    if (PathIsNetworkPathW(path))
        return out->load(path);
    if (PathGetDriveNumberW(path) != -1)
        return out->load(path);
    String full = m_directory + path;
    return out->load(full.m_buffer);
}

int DataFileProvider::writeFile(FileTree* file, aStream* in, bool encrypt)
{
    Bytes data;
    int result;
    if (!encrypt)
    {
        aStream& source = *in;
        static_cast<aStream&>(data).assign(source.m_data, source.m_size);
        result = data.aStream::save(FileTree_path(file).m_buffer);
    }
    else if (!encryptDataFile(in, FileTree_key(file), data))
    {
        result = data.aStream::save(FileTree_path(file).m_buffer);
    }
    else
    {
        result = E_FAIL;
    }
    return result;
}

int DataFileProvider::open(HWND parent, FolderRoot* root)
{
    m_directory = FolderTree_path(root);

    BrowseFoldersDialog dialog(root);
    dialog.m_buttons = 7;
    RECT rect;
    SetRect(&rect, 0x80000000, 0x80000000, 300, 150);
    int result;
    if (dialog.create(parent, 0, &rect, 0x80C80000, 0x10101, L"Opening Workspace"))
    {
        dialog.m_owner = Modal_getTopLevelWindow(parent);
        Window_centerOn(dialog.m_hWnd, dialog.m_owner);
        dialog.show(true);
        if (dialog.doModal() == 1)
        {
            result = 0;
        }
        else
        {
            root->reset();
            result = E_ABORT;
        }
    }
    else
    {
        result = E_FAIL;
    }
    return result;
}

int DataFileProvider::readRaw(FileTree* file, void* buffer, DWORD* size)
{
    FILE* stream = 0;
    int result = _wfopen_s(&stream, FileTree_path(file).m_buffer, L"rb");
    if (result || !stream)
        result = 0x80030002;
    int length = 0;
    if (!result)
    {
        struct _stat64 info;
        if (_fstat64(_fileno(stream), &info) >= 0)
        {
            if (info.st_size <= 0x7FFFFFFF)
            {
                length = (int)info.st_size;
                if (*size >= (DWORD)info.st_size)
                {
                    if (fread(buffer, (size_t)info.st_size, 1, stream) != 1)
                        result = 0x8003001E;
                }
                else
                {
                    result = 0x80090028;
                }
            }
            else
            {
                result = 0x80030001;
            }
        }
        else
        {
            result = 6;
        }
    }
    if (stream)
        fclose(stream);
    *size = length;
    return result;
}
