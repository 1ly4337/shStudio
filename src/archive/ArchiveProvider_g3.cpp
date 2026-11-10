#include "ArchiveProvider.h"
#include <shlwapi.h>
#include <commctrl.h>
#include "String.hpp"
#include "Bytes.h"
#include "FolderRoot.h"
#include "Cell.h"
#include "Window.h"
#include "TreeList.h"
#include "ReadArchiveDialog.h"
#include "ContentFiles.h"
#include "ArchiveTree_g3.h"
#include "ida_compat.h"
#include "_imports_p09.h"
#include "_imports_g3.h"

int ArchiveProvider::readFile(FileTree* file, iStream* out, bool decrypt)
{
    if (!FileTree_offset(file) && !FileTree_size(file))
    {
        String* name = &FileTree_name(file);
        if (FileTree_path(file).m_length)
            name = &FileTree_path(file);
        if (!name->endsWithNoCase(L"mlx"))
            return loadDataFile(file, out, decrypt);
    }
    int result = m_archive.readFile(file, *out);
    if (!result && decrypt)
    {
        aStream* data = out ? static_cast<aStream*>(out) : 0;
        if (!sub_418C00(data, 0, FileTree_key(file)))
            return E_FAIL;
    }
    return result;
}

int ArchiveProvider::readPath(const wchar_t* path, iStream* out)
{
    if (!path)
        return 161;
    if (*path != L'\\' && PathFileExistsW(path))
        return out->load(path);
    FileTree* file = (FileTree*)m_root->findFile(path);
    if (file)
        return m_archive.readFile(file, *out);
    return E_FAIL;
}

int ArchiveProvider::writeFile(FileTree* file, aStream* in, bool encrypt)
{
    Bytes data;
    int result;
    if (!encrypt)
    {
        aStream& source = *in;
        static_cast<aStream&>(data).assign(source.m_data, source.m_size);
        result = m_archive.writeFile(file, data);
    }
    else if (!encryptDataFile(in, FileTree_key(file), data))
    {
        result = m_archive.writeFile(file, data);
    }
    else
    {
        result = E_FAIL;
    }
    return result;
}

int ArchiveProvider::open(HWND parent, FolderRoot* root)
{
    m_directory = FolderTree_path(root);
    DWORD result = m_archive.open(m_directory.m_buffer);
    if (!result)
    {
        {
            ReadArchiveDialog dialog(root, &m_archive);
            dialog.create(parent);
            if (dialog.doModal() != 1)
            {
                root->reset();
                result = E_ABORT;
            }
        }
        if (!result)
        {
            String deathEater(L"Character\\DeathEater\\");
            addArchiveFile(root, deathEater, L"demf.mlx", m_archive.m_codePage);
            addArchiveFile(root, deathEater, L"dewf.mlx", m_archive.m_codePage);
            addArchiveFile(root, deathEater, L"demr.mlx", m_archive.m_codePage);
            addArchiveFile(root, deathEater, L"dewr.mlx", m_archive.m_codePage);
            String elf(L"Character\\Elf\\");
            addArchiveFile(root, elf, L"elmm.mlx", m_archive.m_codePage);
            addArchiveFile(root, elf, L"elwm.mlx", m_archive.m_codePage);
            addArchiveFile(root, elf, L"elmr.mlx", m_archive.m_codePage);
            addArchiveFile(root, elf, L"elwr.mlx", m_archive.m_codePage);
            String human(L"Character\\Human\\");
            addArchiveFile(root, human, L"humf.mlx", m_archive.m_codePage);
            addArchiveFile(root, human, L"huwf.mlx", m_archive.m_codePage);
            addArchiveFile(root, human, L"humm.mlx", m_archive.m_codePage);
            addArchiveFile(root, human, L"huwm.mlx", m_archive.m_codePage);
            String vile(L"Character\\Vile\\");
            addArchiveFile(root, vile, L"vimm.mlx", m_archive.m_codePage);
            addArchiveFile(root, vile, L"viwm.mlx", m_archive.m_codePage);
            addArchiveFile(root, vile, L"vimr.mlx", m_archive.m_codePage);
            addArchiveFile(root, vile, L"viwr.mlx", m_archive.m_codePage);
        }
    }
    return result;
}

int ArchiveProvider::readRaw(FileTree* file, void* buffer, DWORD* size)
{
    return m_archive.readRaw(file, buffer, size);
}

int ArchiveProvider::addFile(Window* tree, FolderTree* folder, wchar_t* fileName)
{
    int error = 0;
    FileTree* file = m_archive.addFile(folder, fileName, &error);
    if (file)
    {
        int type = getContentFileType(fileName);
        LPARAM parentItem = folder->m_data;
        Cell* cell = new Cell(file);
        HTREEITEM item = static_cast<TreeList*>(tree)->insertCell(cell, (HTREEITEM)parentItem, type, 0, TVI_LAST);
        FileTree_type(file) = type;
        file->m_data = (int)item;
        SendMessageW(FIELD(HWND, tree, 0x38), TVM_SELECTITEM, TVGN_CARET, (LPARAM)item);
    }
    return error;
}
