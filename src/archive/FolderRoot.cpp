#include <windows.h>
#include <cwchar>
#include "FolderRoot.h"

FolderRoot::FolderRoot()
    : FolderTree(NULL, NULL, 0)
{
}

void FolderRoot::reset()
{
    FolderTree::reset();
}

static void stripRootPrefix(const FolderTree* root, String& rel)
{
    const wchar_t* rootPath = root->m_path.m_buffer;
    if (rootPath)
    {
        size_t len = wcslen(rootPath);
        if (len <= rel.m_length && wcsncmp(rel.m_buffer, rootPath, len) == 0)
        {
            rel = rel.right(rel.m_length - root->m_path.m_length);
            return;
        }
    }
    if (rel.m_length >= 2 && wcsncmp(rel.m_buffer, L"\\\\", 2) == 0)
        rel = rel.trimLeft(L'\\');
}

static FolderTree* findSubFolder(FolderTree* folder, const String& name)
{
    FolderTree::FoldersTree& folders = folder->m_folders;
    int count = folders.size();
    for (int i = 0; i < count; ++i)
    {
        FolderTree* sub = folders.get(i);
        if (sub->m_name.m_length == name.m_length &&
            _wcsnicmp(sub->m_name.m_buffer, name.m_buffer, sub->m_name.m_length) == 0)
            return folders.get(i);
    }
    return 0;
}

FileTree* FolderRoot::findFile(const wchar_t* path)
{
    if (!path)
        return 0;
    if (path[0] == L'd' && path[1] == L'a' && path[2] == L't' && path[3] == L'a' && path[4] == L'\\')
        path += 5;

    String rel;
    rel.append(path);
    stripRootPrefix(this, rel);

    FolderTree* folder = this;
    for (;;)
    {
        unsigned slash = 0;
        while (slash < rel.m_length && rel.m_buffer[slash] != L'\\')
            ++slash;
        if (slash == rel.m_length)
        {
            FolderTree::FilesTree& files = folder->m_files;
            int count = files.size();
            for (int i = 0; i < count; ++i)
            {
                FileTree* file = files.get(i);
                if (rel.m_length == file->m_name.m_length &&
                    _wcsnicmp(rel.m_buffer, file->m_name.m_buffer, rel.m_length) == 0)
                    return files.get(i);
            }
            return 0;
        }

        String head = rel.left(slash);
        rel = rel.right(rel.m_length - slash - 1);
        if (head.m_length)
            folder = findSubFolder(folder, head);
        if (!folder)
            return 0;
    }
}

DWORD FolderRoot::rescan(StreamNotifier* notifier)
{
    DWORD result = FolderTree::scan(notifier);
    if (result)
        return result;
    computeSize();
    return 0;
}

FolderTree* FolderRoot::findFolderPath(const wchar_t* path)
{
    if (!path)
        return 0;

    String rel;
    rel.append(path);
    stripRootPrefix(this, rel);

    FolderTree* folder = this;
    for (;;)
    {
        if (!rel.m_length)
            return folder;
        unsigned slash = 0;
        while (slash < rel.m_length && rel.m_buffer[slash] != L'\\')
            ++slash;
        if (slash == rel.m_length)
            return 0;

        String head = rel.left(slash);
        rel = rel.right(rel.m_length - slash - 1);
        folder = findSubFolder(folder, head);
        if (!folder)
            return 0;
    }
}
