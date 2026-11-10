#include <windows.h>
#include <shlwapi.h>
#include <cstring>
#include <cwchar>
#include "FolderTree.h"
#include "iStream.hpp"
#include "oStream.hpp"
#include "Exception.h"
#include "StreamNotifier.h"

#pragma warning(disable : 4066)

static int compareLogical(const wchar_t* a, const wchar_t* b)
{
    if (!a)
        return -(b != 0);
    if (!b)
        return 1;
    return StrCmpLogicalW(a, b);
}

FolderTree::FolderTree()
    : m_path()
    , m_name()
    , m_folders()
    , m_files()
{
    m_totalSize = 0;
    m_codePage = 0;
}

FolderTree::FolderTree(const wchar_t* name, const wchar_t* path, int codePage)
    : m_path()
    , m_name()
    , m_folders()
    , m_files()
{
    m_name = name;
    m_path = path;
    m_codePage = codePage;
    m_totalSize = 0;
}

FolderTree::~FolderTree()
{
}

const wchar_t* FolderTree::getString(DWORD tag) const
{
    if (tag == 'path')
        return m_path.m_buffer;
    if (tag == 'name')
        return m_name.m_buffer;
    return 0;
}

int FolderTree::getInt(DWORD tag) const
{
    switch (tag)
    {
    case 'size':
        return -1;
    case '?chl':
    {
        bool hasChildren = m_files.size() || m_folders.size();
        return hasChildren ? 1 : 0;
    }
    case 'chlc':
        return m_files.size() + m_folders.size();
    default:
        return 0;
    }
}

void FolderTree::reset()
{
    m_path.clear();
    m_name.clear();
    m_folders.deleteAll();
    m_files.deleteAll();
    m_totalSize = 0;
}

int FolderTree::compare(const Node* otherNode, DWORD tag) const
{
    const FolderTree* other = dynamic_cast<const FolderTree*>(otherNode);
    if (tag != 'name')
        return 0;
    return compareLogical(m_name.m_buffer, other->m_name.m_buffer);
}

DWORD FolderTree::scan(StreamNotifier* notifier)
{
    WIN32_FIND_DATAW data;
    memset(&data, 0, sizeof(data));
    HANDLE find = FindFirstFileW((m_path + L"*.*").m_buffer, &data);
    if (find == INVALID_HANDLE_VALUE)
        return GetLastError();

    DWORD result;
    for (result = addFindData(&data, notifier); !result; result = addFindData(&data, notifier))
    {
        if (!FindNextFileW(find, &data))
            break;
    }
    FindClose(find);
    if (!result)
    {
        int files = m_files.m_count;
        if (files >= 2)
        {
            m_files.m_changes = 0;
            m_files.sort(0, files - 1, 0);
        }
        int folders = m_folders.m_count;
        if (folders >= 2)
        {
            m_folders.m_changes = 0;
            m_folders.sort(0, folders - 1, 0);
        }
    }
    return result;
}

FileTree* FolderTree::findFile(const wchar_t* name)
{
    int files = m_files.size();
    for (int i = 0; i < files; ++i)
    {
        FileTree* file = m_files.get(i);
        bool match;
        if (name && *name)
        {
            if (wcslen(name) != file->m_name.m_length)
                continue;
            match = _wcsnicmp(file->m_name.m_buffer, name, file->m_name.m_length) == 0;
        }
        else
        {
            match = file->m_name.m_length == 0;
        }
        if (match)
            return file;
    }
    int folders = m_folders.size();
    for (int i = 0; i < folders; ++i)
    {
        FileTree* found = m_folders.get(i)->findFile(name);
        if (found)
            return found;
    }
    return 0;
}

FileTree* FolderTree::createFile()
{
    return new FileTree();
}

FolderTree* FolderTree::createFolder()
{
    return new FolderTree();
}

int FolderTree::compareName(const FolderTree* other) const
{
    return compareLogical(m_name.m_buffer, other->m_name.m_buffer);
}

void FolderTree::read(iStream& in, DWORD fmt)
{
    readTree(in, fmt, 0);
}

void FolderTree::write(oStream& out, DWORD fmt) const
{
    writeTree(out, fmt, 0);
}

bool FolderTree::toText(String& out, DWORD fmt) const
{
    if (fmt == 'dump')
    {
        out.append(m_name.m_buffer, m_name.m_length) << L'\\';
    }
    else
    {
        out << L'"';
        out.append(m_name.m_buffer, m_name.m_length) << L'"';
    }
    out << L"\r\n";
    ++out.m_indent;
    out << L'\t';

    bool ok = true;
    int folders = m_folders.size();
    for (int i = 0; i < folders && ok; ++i)
    {
        FolderTree* folder = m_folders.get(i);
        if (folder)
            ok = folder->toText(out, fmt);
    }
    int files = m_files.size();
    for (int i = 0; i < files && ok; ++i)
    {
        FileTree* file = m_files.get(i);
        if (file)
            ok = file->toText(out, fmt);
    }

    --out.m_indent;
    if (out.m_buffer && out.m_buffer[out.m_length - 1] == L'\t')
        out.m_buffer[out.m_length] = 0;
    if (out.m_buffer && out.m_buffer[out.m_length - 1] == L'\t')
    {
        --out.m_length;
        out.m_buffer[out.m_length] = 0;
    }
    return ok;
}

int FolderTree::countFiles() const
{
    int count = m_files.size();
    int folders = m_folders.size();
    for (int i = 0; i < folders; ++i)
        count += m_folders.get(i)->countFiles();
    return count;
}

void FolderTree::readTree(iStream& in, DWORD fmt, StreamNotifier* notifier)
{
    m_totalSize = 0;
    m_folders.deleteAll();
    m_files.deleteAll();
    m_name.read(in, 4, static_cast<aStream&>(in).m_codePage);
    if (m_name.m_length)
        m_path.append(m_name.m_buffer, m_name.m_length) << L'\\';
    if (notifier)
        notifier->setStatus(m_path.m_buffer);

    FileTree* file = 0;
    try
    {
        int files = (int)in.readInt();
        for (int i = 0; i < files; ++i)
        {
            file = createFile();
            file->read(in, fmt);
            if (notifier && !notifier->progress(file->m_name.m_buffer, 1, 0))
                throw 1;
            file->m_path = m_path + file->m_name;
            file->m_parent = this;
            file->m_codePage = m_codePage;
            m_totalSize += file->m_size;
            m_files.add(file);
        }
        int count = m_files.m_count;
        if (count >= 2)
        {
            m_files.m_changes = 0;
            m_files.sort(0, count - 1, 0);
        }
    }
    catch (...)
    {
        if (file)
            delete file;
        throw 0;
    }

    FolderTree* folder = 0;
    try
    {
        aStream& s = in;
        unsigned long folders = in.peekInt();
        int next = s.m_pos + 4;
        if (next > s.m_size)
            throw ArrayIndexOutOfBoundsException(next, s.m_size);
        s.m_pos = next;
        if (folders > 0x400)
            throw 2;
        for (int i = 0; i < (int)folders; ++i)
        {
            folder = createFolder();
            folder->m_path = m_path;
            folder->m_codePage = m_codePage;
            folder->readTree(in, fmt, notifier);
            m_totalSize += folder->m_totalSize;
            m_folders.add(folder);
        }
    }
    catch (...)
    {
        if (folder)
            delete folder;
        throw 1;
    }
}

void FolderTree::writeTree(oStream& out, DWORD fmt, StreamNotifier* notifier) const
{
    int files = m_files.size();
    out << files;
    for (int i = 0; i < files; ++i)
        m_files.get(i)->write(out, fmt);
    int folders = m_folders.size();
    out << folders;
    for (int i = 0; i < folders; ++i)
    {
        FolderTree* folder = m_folders.get(i);
        folder->m_name.write(out, 4, static_cast<aStream&>(out).m_codePage);
        folder->writeTree(out, fmt, notifier);
    }
}

FolderTree* FolderTree::findChildFolder(const wchar_t* name) const
{
    int folders = m_folders.size();
    for (int i = 0; i < folders; ++i)
    {
        FolderTree* folder = m_folders.m_items[i];
        bool match;
        if (name && *name)
        {
            if (wcslen(name) != folder->m_name.m_length)
                continue;
            match = _wcsnicmp(folder->m_name.m_buffer, name, folder->m_name.m_length) == 0;
        }
        else
        {
            match = folder->m_name.m_length == 0;
        }
        if (match)
            return folder;
    }
    return 0;
}

FileTree* FolderTree::findChildFile(const wchar_t* name) const
{
    int files = m_files.size();
    for (int i = 0; i < files; ++i)
    {
        FileTree* file = m_files.get(i);
        bool match;
        if (name && *name)
        {
            if (wcslen(name) != file->m_name.m_length)
                continue;
            match = _wcsnicmp(file->m_name.m_buffer, name, file->m_name.m_length) == 0;
        }
        else
        {
            match = file->m_name.m_length == 0;
        }
        if (match)
            return m_files.get(i);
    }
    return 0;
}

FileTree* FolderTree::addFile(const wchar_t* name, __int64 size)
{
    FileTree* file = createFile();
    if (file)
    {
        file->m_name = name;
        file->m_size = size;
        file->m_codePage = m_codePage;
    }
    return file;
}

FolderTree* FolderTree::addFolder(const wchar_t* name, const wchar_t* parentPath)
{
    FolderTree* folder = createFolder();
    if (folder)
    {
        folder->m_name = name;
        folder->m_path = parentPath;
        folder->m_path << name << L'\\';
        folder->m_totalSize = 0;
        folder->m_codePage = m_codePage;
    }
    return folder;
}

DWORD FolderTree::addFindData(const WIN32_FIND_DATAW* data, StreamNotifier* notifier)
{
    if (!(data->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
    {
        if (data->dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
            return 0;
        int sizeLow = (int)data->nFileSizeLow;
        int sizeHigh = (int)data->nFileSizeHigh;
        if (notifier && !notifier->progress(data->cFileName, sizeLow, sizeHigh))
            return ERROR_CANCELLED;
        __int64 size = ((__int64)(DWORD)sizeHigh << 32) | (DWORD)sizeLow;
        FileTree* file = addFile(data->cFileName, size);
        file->m_path = m_path + data->cFileName;
        file->m_time = &data->ftLastWriteTime;
        m_files.add(file);
        m_totalSize += size;
        return 0;
    }

    if (data->cFileName[0] == L'.')
        return 0;
    DWORD result;
    FolderTree* folder = addFolder(data->cFileName, m_path.m_buffer);
    if (folder)
    {
        if (!notifier || notifier->setStatus(folder->m_path.m_buffer))
            result = folder->scan(notifier);
        else
            result = ERROR_CANCELLED;
    }
    else
    {
        result = (DWORD)E_FAIL;
    }
    m_folders.add(folder);
    m_totalSize += folder->m_totalSize;
    return result;
}

__int64 FolderTree::computeSize()
{
    m_totalSize = 0;
    int files = m_files.size();
    for (int i = 1; i < files; ++i)
        m_totalSize += m_files.get(i)->m_size;
    int folders = m_folders.size();
    for (int i = 0; i < folders; ++i)
        m_totalSize += m_folders.get(i)->computeSize();
    return m_totalSize;
}

void FolderTree::FilesTree::sort(int lo, int hi, NodeComparator* cmp)
{
    while (lo < hi)
    {
        int i = lo;
        int j = hi;
        FileTree* pivot = m_items[(lo + hi) / 2];
        do
        {
            for (; i < hi; ++i)
            {
                FileTree* item = m_items[i];
                int c;
                if (item)
                {
                    if (!pivot)
                        continue;
                    c = cmp ? cmp->compare(item, pivot) : compareLogical(item->m_name.m_buffer, pivot->m_name.m_buffer);
                }
                else
                {
                    c = pivot != 0;
                }
                if (c >= 0)
                    break;
            }
            for (; j > 0; --j)
            {
                FileTree* item = m_items[j];
                int c;
                if (item)
                {
                    if (!pivot)
                        break;
                    c = cmp ? cmp->compare(item, pivot) : compareLogical(item->m_name.m_buffer, pivot->m_name.m_buffer);
                }
                else
                {
                    c = pivot != 0;
                }
                if (c <= 0)
                    break;
            }
            if (j < i)
                break;
            if (j != i)
            {
                FileTree* t = m_items[j];
                m_items[j] = m_items[i];
                m_items[i] = t;
                ++m_changes;
            }
            --j;
            ++i;
        } while (i <= j);
        if (lo < j)
            sort(lo, j, cmp);
        if (i >= hi)
            return;
        lo = i;
    }
}

void FolderTree::FoldersTree::sort(int lo, int hi, NodeComparator* cmp)
{
    while (lo < hi)
    {
        int i = lo;
        int j = hi;
        FolderTree* pivot = m_items[(lo + hi) / 2];
        do
        {
            for (; i < hi; ++i)
            {
                FolderTree* item = m_items[i];
                int c;
                if (item)
                {
                    if (!pivot)
                        continue;
                    c = cmp ? cmp->compare(item, pivot) : item->compareName(pivot);
                }
                else
                {
                    c = pivot != 0;
                }
                if (c >= 0)
                    break;
            }
            for (; j > 0; --j)
            {
                FolderTree* item = m_items[j];
                int c;
                if (item)
                {
                    if (!pivot)
                        break;
                    c = cmp ? cmp->compare(item, pivot) : item->compareName(pivot);
                }
                else
                {
                    c = pivot != 0;
                }
                if (c <= 0)
                    break;
            }
            if (j < i)
                break;
            if (j != i)
            {
                FolderTree* t = m_items[j];
                m_items[j] = m_items[i];
                m_items[i] = t;
                ++m_changes;
            }
            --j;
            ++i;
        } while (i <= j);
        if (lo < j)
            sort(lo, j, cmp);
        if (i >= hi)
            return;
        lo = i;
    }
}
