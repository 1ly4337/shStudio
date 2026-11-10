#include "ArchiveExtractor.h"
#include "Exception.h"
#include "StreamNotifier.h"
#include "SahArchive.h"
#include "ArchiveTree_g3.h"
#include "_imports_p09.h"
#include "_imports_g3.h"

ArchiveExtractor::ArchiveExtractor()
{
    m_file = INVALID_HANDLE_VALUE;
    m_fileSize.QuadPart = 0;
    m_cancelled = false;
    m_buffer = new BYTE[0x800000];
    m_unused6C = 0;
    m_fromTree = false;
    m_extracted = 0;
    m_unused74 = 0;
    m_errors.m_radix = 16;
}

ArchiveExtractor::~ArchiveExtractor()
{
    if (!m_fromTree && m_file != INVALID_HANDLE_VALUE)
        CloseHandle(m_file);
    delete[] m_buffer;
}

DWORD ArchiveExtractor::open(const wchar_t* safPath)
{
    if (!m_fromTree && m_file != INVALID_HANDLE_VALUE)
        CloseHandle(m_file);
    m_file = INVALID_HANDLE_VALUE;

    String path(safPath);
    wchar_t* name = path.m_buffer;
    unsigned last = path.m_length - 1;
    if (path.m_length)
        name[last] = L'h';

    int error = m_index.load(name);
    if (error)
    {
        m_errors << "The indexes (data.sah file) cannot be read.\nReceived error was: " << (long)error << L"\r\n";
        return error;
    }

    SahHeader header;
    m_index.read(&header, 51);
    if (!(*(WORD*)header.magic == 0x4153 && header.magic[2] == 'H'))
    {
        static_cast<aStream&>(m_index).m_pos = 0;
        Bytes plain;
        decodeSahIndex(m_index, plain);
        m_index = plain;
        m_index.read(&header, 51);
        if (!(*(WORD*)header.magic == 0x4153 && header.magic[2] == 'H'))
        {
            m_errors << "The indexes (data.sah file) are invalid.\nThe file doesn't start with 'SAH'.\n";
            return 13;
        }
    }

    if (last < path.m_length)
        name[last] = L'f';
    m_file = CreateFileW(name, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (m_file == INVALID_HANDLE_VALUE)
    {
        DWORD lastError = GetLastError();
        m_errors << "The data.saf file cannot be opened for reading.\nReceived error was: " << (long)lastError << L"\r\n";
        return lastError;
    }
    LARGE_INTEGER fileSize;
    fileSize.QuadPart = 0;
    GetFileSizeEx(m_file, &fileSize);
    m_fileSize = fileSize;
    m_fromTree = false;
    return 0;
}

int ArchiveExtractor::extract(FolderTree* root, const wchar_t* destination, StreamNotifier* notifier)
{
    if (m_file == INVALID_HANDLE_VALUE)
        return E_UNEXPECTED;

    m_destination = destination;
    if (!m_destination.m_length || m_destination.m_buffer[m_destination.m_length - 1] != L'\\')
        m_destination << L'\\';
    if (!m_fromTree)
        FolderTree_totalSize(root) = m_fileSize.QuadPart;

    int result;
    try
    {
        if (notifier)
            notifier->setRange((LPARAM)(FolderTree_totalSize(root) >> 16));
        result = extractFolder(root, m_destination, notifier);
    }
    catch (...)
    {
        result = E_FAIL;
    }
    if (!m_fromTree && m_file != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_file);
        m_file = INVALID_HANDLE_VALUE;
    }
    return result;
}

int ArchiveExtractor::extractFolder(FolderTree* folder, const String& parentDir, StreamNotifier* notifier)
{
    String path(parentDir);
    if (!path.m_length || path.m_buffer[path.m_length - 1] != L'\\')
        path << L'\\';

    if (!m_fromTree)
    {
        try
        {
            FolderTree_name(folder).read(m_index, 4, static_cast<aStream&>(m_index).m_codePage);
        }
        catch (...)
        {
            m_errors << "The name of a directory in data.sah is invalid.\n";
            return E_FAIL;
        }
        FolderTree_path(folder) = path + FolderTree_name(folder);
        String& folderPath = FolderTree_path(folder);
        if (!folderPath.m_length || folderPath.m_buffer[folderPath.m_length - 1] != L'\\')
            folderPath << L'\\';
    }

    String& folderName = FolderTree_name(folder);
    if (folderName.m_length)
        path.append(folderName.m_buffer, folderName.m_length) << L'\\';

    DWORD lastError;
    if (!CreateDirectoryW(path.m_buffer, 0)
        && (lastError = GetLastError(), lastError != ERROR_ALREADY_EXISTS) && lastError)
    {
        m_errors << "The directory \"" << path << "\" cannot be created.\nReceived error was " << (long)lastError;
        return 267;
    }

    if (notifier && !notifier->setStatus(path.m_buffer))
        m_cancelled = true;

    try
    {
        ptrArray<FileTree*>& files = FolderTree_files(folder);
        int fileCount = m_fromTree ? files.size() : (int)m_index.readInt();
        for (int i = 0; i < fileCount; ++i)
        {
            FileTree* file;
            if (m_fromTree)
            {
                file = files.m_items[i];
            }
            else
            {
                file = m_index.iStream::readFileTree();
                if (!file)
                {
                    m_errors << "A file record in data.sah is invalid.";
                    return E_FAIL;
                }
                FileTree_path(file) = path + FileTree_name(file);
                files.add(file);
            }
            if (notifier)
            {
                __int64 units = FileTree_size(file) >> 16;
                if (!notifier->progress(FileTree_name(file).m_buffer, (int)units, (int)(units >> 32)))
                    m_cancelled = true;
            }
            if (m_cancelled)
                return 1223;
            if (!extractFile(file, path))
                ++m_extracted;
        }

        ptrArray<FolderTree*>& folders = FolderTree_folders(folder);
        int folderCount = m_fromTree ? folders.size() : (int)m_index.readInt();
        for (int i = 0; i < folderCount; ++i)
        {
            FolderTree* sub;
            if (m_fromTree)
            {
                sub = folders.m_items[i];
            }
            else
            {
                sub = new FolderTree(0, 0, FolderTree_context(folder));
                folders.add(sub);
            }
            int result = extractFolder(sub, path, notifier);
            if (result)
                return result;
        }
    }
    catch (...)
    {
        return E_FAIL;
    }
    return 0;
}

int ArchiveExtractor::extractFile(FileTree* file, const String& dir)
{
    int result = 0;
    LARGE_INTEGER offset;
    offset.QuadPart = FileTree_offset(file);
    if (!SetFilePointerEx(m_file, offset, 0, FILE_BEGIN))
    {
        m_errors << "The offset of the file \"" << FileTree_name(file)
                 << "\" is invalid; the file is not extracted.\nRead value was " << (__int64)offset.QuadPart << L"\r\n";
        return 25;
    }

    HANDLE output = CreateFileW((dir + FileTree_name(file)).m_buffer, GENERIC_WRITE, 0, 0, CREATE_ALWAYS,
                                FILE_ATTRIBUTE_NORMAL, 0);
    if (output == INVALID_HANDLE_VALUE)
    {
        DWORD lastError = GetLastError();
        {
            String full = dir + FileTree_name(file);
            m_errors << "The file \"" << full << "\" cannot be created.\n";
        }
        if (lastError == ERROR_INVALID_NAME)
        {
            m_errors << "The name is invalid for this system. (ERROR_INVALID_NAME)";
            return ERROR_INVALID_NAME;
        }
        m_errors << "Received error: " << (long)lastError;
        return lastError;
    }

    DWORD remaining = (DWORD)FileTree_size(file);
    DWORD written;
    if (remaining)
    {
        while (!result)
        {
            if (m_cancelled)
            {
                result = 1223;
            }
            else
            {
                DWORD chunk = 0x800000;
                if (remaining < 0x800000)
                    chunk = remaining;
                DWORD read = chunk;
                if (ReadFile(m_file, m_buffer, chunk, &read, 0) <= 0)
                {
                    m_errors << "The file \"" << FileTree_name(file) << "\" cannot be read from archive.\n";
                    result = 30;
                    break;
                }
                if (!read)
                {
                    m_errors << "The file \"" << FileTree_name(file) << "\" cannot be read from archive.\n";
                    result = 25;
                    break;
                }
                written = 0;
                WriteFile(output, m_buffer, read, &written, 0);
                if (read != written)
                {
                    String full = dir + FileTree_name(file);
                    m_errors << "Fails to write " << (int)read << " bytes to file \"" << full << "\".\n";
                    result = 29;
                    break;
                }
                remaining -= written;
            }
            if (!remaining)
                break;
        }
    }
    CloseHandle(output);
    if (result)
        DeleteFileW(FileTree_path(file).m_buffer);
    return result;
}
