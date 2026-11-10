#include "ArchiveWriter.h"
#include "String.hpp"
#include "Exception.h"
#include "StreamNotifier.h"
#include "SahArchive.h"
#include "ArchiveTree_g3.h"
#include "_imports_p09.h"
#include "_imports_g3.h"

int ArchiveWriter::writeFile(FileTree* file)
{
    LARGE_INTEGER fileSize;
    fileSize.QuadPart = 0;
    if (!GetFileSizeEx(m_file, &fileSize))
        return E_FAIL;
    FileTree_offset(file) = fileSize.QuadPart;

    Bytes data;
    int result;
    if (data.aStream::load(FileTree_path(file).m_buffer))
    {
        result = 161;
    }
    else
    {
        aStream& s = data;
        LARGE_INTEGER* size = (LARGE_INTEGER*)&FileTree_size(file);
        size->HighPart = 0;
        DWORD length = s.m_size;
        size->LowPart = length;
        DWORD written = 0;
        if (WriteFile(m_file, s.m_data, length, &written, 0) && written == length)
        {
            file->write(static_cast<oStream&>(m_index), 'file');
            ++m_fileCount;
            result = 0;
        }
        else
        {
            result = E_FAIL;
        }
    }
    return result;
}

int ArchiveWriter::writeFolder(FolderTree* folder)
{
    if (m_notifier)
    {
        String full = FolderTree_path(folder) + FolderTree_name(folder);
        m_notifier->setStatus(full.m_buffer + m_rootLength);
    }

    oStream& index = m_index;
    ptrArray<FileTree*>& files = FolderTree_files(folder);
    int fileCount = files.size();
    index << (unsigned long)fileCount;
    for (int i = 0; i < fileCount; ++i)
    {
        FileTree* file = files.get(i);
        if (file)
        {
            int result = writeFile(file);
            if (result)
                return result;
            if (m_notifier)
            {
                __int64 units = FileTree_size(file) >> 16;
                if (!m_notifier->progress(FileTree_name(file).m_buffer, (int)units, (int)(units >> 32)))
                    return 1223;
            }
        }
    }

    ptrArray<FolderTree*>& folders = FolderTree_folders(folder);
    int folderCount = folders.size();
    index << (unsigned long)folderCount;
    for (int i = 0; i < folderCount; ++i)
    {
        FolderTree* sub = folders.get(i);
        if (sub)
        {
            FolderTree_name(sub).write(index, 4, static_cast<aStream&>(index).m_codePage);
            int result = writeFolder(sub);
            if (result)
                return result;
        }
    }
    return 0;
}

int ArchiveWriter::write(FolderTree* root, StreamNotifier* notifier)
{
    m_notifier = notifier;
    m_rootLength = FolderTree_path(root).m_length;
    m_index.wipe();

    SahHeader header;
    oStream& index = m_index;
    index.put(&header, 51);

    String signature(L"");
    signature.write(index, 4, static_cast<aStream&>(index).m_codePage);

    m_fileCount = 0;
    int result = writeFolder(root);
    if (!result)
    {
        index << (unsigned long)0 << (unsigned long)0;
        aStream& s = m_index;
        s.m_pos = 0;
        *(int*)(s.m_data + s.m_pos + 7) = m_fileCount;
    }
    if (m_file != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_file);
        m_file = INVALID_HANDLE_VALUE;
    }
    return result;
}
