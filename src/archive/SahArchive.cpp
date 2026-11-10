#include "SahArchive.h"
#include <shlwapi.h>
#include "Exception.h"
#include "FolderRoot.h"
#include "ArchiveTree_g3.h"
#include "_imports_p09.h"
#include "_imports_g3.h"

void decodeSahIndex(Bytes& in, Bytes& out)
{
    aStream& dst = out;
    dst.wipe();
    int done = 0;
    aStream& src = in;
    BYTE key[16] = { 0xFE, 0xFF, 0xFC, 0xFD, 0xFA, 0xFB, 0xF8, 0xF9,
                     0xF6, 0xF7, 0xF4, 0xF5, 0xF2, 0xF3, 0xF0, 0xF1 };
    src.m_pos = 0;
    int size = src.m_size;
    if (size > 16)
    {
        int limit = 16;
        for (;;)
        {
            BYTE block[16];
            in.read(block, 16);
            for (int i = 0; i < 16; ++i)
            {
                BYTE k = key[i];
                BYTE b = (BYTE)(k ^ block[i]);
                block[i] = (BYTE)((b << 4) | (b >> 4));
                key[i] = (BYTE)(k - 16);
            }
            out.put(block, 16);
            done += 16;
            limit += 16;
            if (limit >= size)
                break;
        }
    }
    if (done < size)
    {
        for (BYTE* k = key;; ++k)
        {
            BYTE b = (BYTE)(*k ^ (BYTE)in.readByte());
            b = (BYTE)((b << 4) | (b >> 4));
            out.put(&b, 1);
            ++done;
            if (done >= size)
                break;
        }
    }
    src.m_pos = 0;
    dst.m_pos = 0;
}

SahArchive::SahArchive(FolderRoot* root, int codePage)
    : m_root(root)
{
    m_writable = false;
    m_fileSize.QuadPart = 0;
    m_fileCount = 0;
    m_codePage = codePage;
    m_file = INVALID_HANDLE_VALUE;
}

SahArchive::~SahArchive()
{
    close();
}

void SahArchive::close()
{
    if (m_file != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_file);
        m_file = INVALID_HANDLE_VALUE;
    }
    m_root->reset();
    m_sahPath.clear();
    m_safPath.clear();
    m_fileSize.QuadPart = 0;
    m_fileCount = 0;
}

DWORD SahArchive::open(LPCWSTR safPath)
{
    close();
    if (!safPath || !PathFileExistsW(safPath))
        return 161;

    m_safPath = safPath;
    m_sahPath = safPath;
    if (m_sahPath.m_length)
        m_sahPath.m_buffer[m_sahPath.m_length - 1] = L'h';

    m_file = CreateFileW(m_safPath.m_buffer, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (m_file == INVALID_HANDLE_VALUE)
    {
        m_file = CreateFileW(m_safPath.m_buffer, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        if (m_file == INVALID_HANDLE_VALUE)
            return GetLastError();
        m_writable = false;
    }
    else
    {
        m_writable = true;
    }

    LARGE_INTEGER fileSize;
    fileSize.QuadPart = 0;
    if (!GetFileSizeEx(m_file, &fileSize))
        return GetLastError();
    m_fileSize = fileSize;

    Bytes data;
    DWORD result = data.aStream::load(m_sahPath.m_buffer);
    if (result == 0)
    {
        try
        {
            SahHeader header;
            data.read(&header, 51);
            if (memcmp(header.magic, "SAH", 3) != 0)
            {
                if (*(WORD*)header.magic == 0xEBCB && (BYTE)header.magic[2] == 0x78)
                {
                    Bytes plain;
                    decodeSahIndex(data, plain);
                    data = plain;
                    data.read(&header, 51);
                    if (*(WORD*)header.magic == 0x4153 && header.magic[2] == 'H')
                        result = 0;
                    else
                        result = 13;
                }
                else
                {
                    result = 13;
                }
            }
            if (result == 0)
            {
                m_fileCount = header.fileCount;
                aStream& s = data;
                data.iStream::readBytes(m_header, s.m_size - s.m_pos - 8);
            }
        }
        catch (...)
        {
            result = E_FAIL;
        }
        if (result == 0)
            return 0;
    }
    close();
    return result;
}

int SahArchive::readIndex(StreamNotifier* notifier)
{
    aStream& header = m_header;
    if (m_file == INVALID_HANDLE_VALUE || !header.m_size)
        return E_UNEXPECTED;
    try
    {
        m_root->FolderTree::readTree(m_header, 'file', notifier);
    }
    catch (...)
    {
        m_root->reset();
        return E_FAIL;
    }
    FileTree* holes = m_root->FolderTree::findChildFile(L"holes.bin");
    if (holes)
    {
        iStream in(false, m_codePage);
        if (!readFile(holes, in))
            m_holes.read(in);
    }
    return 0;
}

int SahArchive::readFile(FileTree* file, iStream& out)
{
    if (!m_file || m_file == INVALID_HANDLE_VALUE)
        return 0x80070006;
    LARGE_INTEGER position;
    position.QuadPart = FileTree_offset(file);
    if (!SetFilePointerEx(m_file, position, 0, FILE_BEGIN))
        return 25;
    if (FileTree_size(file) > 0x7FFFFFFF)
        return 13;
    int size = (int)FileTree_size(file);
    aStream& s = out;
    if (!s.allocate(size))
        return 8;
    s.m_codePage = m_codePage;
    DWORD read = 0;
    if (ReadFile(m_file, s.m_data, size, &read, 0) <= 0 || read != (DWORD)size)
        return 30;
    int length = s.m_capacity;
    if (size < length)
        length = size;
    s.m_size = length;
    return 0;
}

int SahArchive::readRaw(FileTree* file, void* buffer, DWORD* size)
{
    if (!m_file || m_file == INVALID_HANDLE_VALUE)
        return 0x80070006;
    LARGE_INTEGER position;
    position.QuadPart = FileTree_offset(file);
    if (!SetFilePointerEx(m_file, position, 0, FILE_BEGIN))
        return 0x80030019;
    if (FileTree_size(file) > 0x7FFFFFFF)
        return 0x80030001;
    DWORD length = (DWORD)FileTree_size(file);
    if (*size < length)
    {
        *size = length;
        return 0x80090028;
    }
    DWORD read = 0;
    if (ReadFile(m_file, buffer, length, &read, 0) > 0 && read == length)
    {
        *size = length;
        return 0;
    }
    return 0x8003001E;
}

int SahArchive::writeAt(HANDLE file, const LARGE_INTEGER* position, const void* data, DWORD size)
{
    if (!file || file == INVALID_HANDLE_VALUE)
        return 0x80070006;
    if (!SetFilePointerEx(file, *position, 0, FILE_BEGIN))
        return 25;
    DWORD written = 0;
    if (WriteFile(file, data, size, &written, 0) && written == size)
        return 0;
    return 29;
}

int SahArchive::writeFile(FileTree* file, oStream& data)
{
    if (!m_file || m_file == INVALID_HANDLE_VALUE)
        return 0x80070006;
    if (!m_writable)
        return 19;
    aStream& s = data;
    DWORD size = s.m_size;
    LARGE_INTEGER* fileSize = (LARGE_INTEGER*)&FileTree_size(file);
    LARGE_INTEGER* fileOffset = (LARGE_INTEGER*)&FileTree_offset(file);
    int result;
    if (size <= fileSize->LowPart)
    {
        LARGE_INTEGER position = *fileOffset;
        result = writeAt(m_file, &position, s.m_data, size);
        if (!result && (size != fileSize->LowPart || fileSize->HighPart))
        {
            fileSize->HighPart = 0;
            fileSize->LowPart = size;
            return saveIndex();
        }
        return result;
    }

    LARGE_INTEGER position;
    position.QuadPart = 0;
    LARGE_INTEGER hole;
    hole.QuadPart = m_holes.findBest(size);
    LONG highPart;
    if (hole.QuadPart)
    {
        m_holes.consume(hole.QuadPart, size);
        position = hole;
        result = writeAt(m_file, &position, s.m_data, size);
        if (result)
            return result;
        fileOffset->LowPart = hole.LowPart;
        highPart = hole.HighPart;
    }
    else
    {
        if (!GetFileSizeEx(m_file, &position))
            return 25;
        result = writeAt(m_file, &position, s.m_data, size);
        if (result)
            return result;
        m_holes.add(fileOffset->QuadPart, fileSize->LowPart);
        fileOffset->LowPart = position.LowPart;
        highPart = position.HighPart;
    }
    fileSize->HighPart = 0;
    fileSize->LowPart = size;
    fileOffset->HighPart = highPart;
    result = saveHoles();
    if (!result)
        return saveIndex();
    return result;
}

FileTree* SahArchive::addFile(FolderTree* folder, WCHAR* path, int* error)
{
    wchar_t* name = PathFindFileNameW(path);
    if (!name)
    {
        *error = 3;
        return 0;
    }
    Bytes data;
    *error = data.aStream::load(path);
    if (!*error)
    {
        if (folder->findFile(name))
        {
            *error = 183;
        }
        else
        {
            FileTree* file = new FileTree(folder, name, m_codePage);
            *error = writeFile(file, data);
            if (!*error)
            {
                FolderTree_files(folder).add(file);
                FileTree_path(file) = FolderTree_path(folder) + name;
                *error = saveIndex();
                return file;
            }
            if (file)
                delete file;
        }
    }
    return 0;
}

int SahArchive::saveIndex()
{
    m_header.wipe();
    m_root->write(m_header, 'file');

    SahHeader header;
    header.fileCount = m_root->FolderTree::countFiles();

    Bytes out;
    out.put(&header, 51);
    BYTE marker[5] = { 1, 0, 0, 0, 0 };
    out.put(marker, 5);
    aStream& index = m_header;
    out.put(index.m_data + index.m_pos, index.m_size - index.m_pos);
    BYTE trailer[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    out.put(trailer, 8);
    return out.aStream::save(m_sahPath.m_buffer);
}

int SahArchive::saveHoles()
{
    oStream out(false, m_codePage);
    out << (unsigned long)m_holes.m_capacity << (unsigned long)m_holes.m_count;
    out.put(m_holes.m_items, 12 * m_holes.m_capacity);

    aStream& s = out;
    int result;
    LARGE_INTEGER position;
    FileTree* holes = m_root->FolderTree::findChildFile(L"holes.bin");
    if (holes)
    {
        if (FileTree_size(holes) == (__int64)(DWORD)s.m_size)
            return writeFile(holes, out);

        position.QuadPart = FileTree_offset(holes);
        DWORD oldSize = (DWORD)FileTree_size(holes);
        m_holes.add(position.QuadPart, oldSize);
        out << (unsigned long)m_holes.m_capacity << (unsigned long)m_holes.m_count;
        out.put(m_holes.m_items, 12 * m_holes.m_capacity);
        BYTE* fill = new BYTE[oldSize];
        memset(fill, 0xBF, oldSize);
        result = writeAt(m_file, &position, fill, oldSize);
        delete[] fill;
        if (result)
            return result;
    }

    if (!GetFileSizeEx(m_file, &position))
        return 25;
    DWORD size = s.m_size;
    result = writeAt(m_file, &position, s.m_data, size);
    if (!result)
    {
        if (!holes)
        {
            holes = m_root->FolderTree::addFile(L"holes.bin", (__int64)(int)size);
            FolderTree_files(m_root).add(holes);
        }
        FileTree_offset(holes) = position.QuadPart;
        result = 0;
    }
    return result;
}

void SahArchive::Holes::add(__int64 offset, int size)
{
    if (m_count == m_capacity)
    {
        Hole* items = new Hole[m_capacity + 128];
        if (m_items)
        {
            memcpy(items, m_items, 12 * m_capacity);
            memset(&items[m_capacity * 12], 0, 0x600);
            delete[] m_items;
            m_capacity += 128;
        }
        else
        {
            memset(items, 0, 0x600);
            m_capacity = 128;
        }
        m_items = items;
    }
    m_items[m_count].offset = offset;
    m_items[m_count++].size = size;
}

void SahArchive::Holes::consume(__int64 offset, int size)
{
    int index = 0;
    int count = m_count;
    if (count <= 0)
        return;
    for (Hole* hole = m_items; hole->offset != offset; ++hole)
    {
        if (++index >= count)
            return;
    }
    if (m_items[index].size - size >= m_items[index].size >> 3)
    {
        m_items[index].offset += size;
        m_items[index].size -= size;
    }
    else
    {
        m_count = count - 1;
        if (index < count - 2)
        {
            do
            {
                m_items[index] = m_items[index + 1];
                ++index;
            } while (index < m_count - 1);
        }
        m_items[m_count].offset = 0;
        m_items[m_count].size = 0;
    }
}

__int64 SahArchive::Holes::findBest(int size)
{
    int count = m_count;
    int best = 0x7FFFFFFF;
    __int64 result = 0;
    if (count <= 0)
        return 0;
    Hole* hole = m_items;
    do
    {
        int holeSize = hole->size;
        if (holeSize >= size && holeSize - size < best)
        {
            result = hole->offset;
            best = holeSize - size;
        }
        ++hole;
        --count;
    } while (count);
    return result;
}

int SahArchive::Holes::read(iStream& in)
{
    m_capacity = in.readInt();
    m_count = in.readInt();
    m_items = new Hole[m_capacity];
    in.read(m_items, 12 * m_capacity);
    return 0;
}
