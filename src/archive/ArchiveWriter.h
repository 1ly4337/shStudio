#pragma once
#include <windows.h>
#include "Bytes.h"

class FolderTree;
class FileTree;
class StreamNotifier;

class ArchiveWriter
{
public:
    ArchiveWriter(HANDLE file) : m_file(file), m_rootLength(0), m_fileCount(0), m_notifier(0) {}

    int writeFile(FileTree* file);
    int writeFolder(FolderTree* folder);
    int write(FolderTree* root, StreamNotifier* notifier);

    HANDLE          m_file;
    Bytes           m_index;
    int             m_rootLength;
    int             m_fileCount;
    StreamNotifier* m_notifier;
};
static_assert(sizeof(ArchiveWriter) == 0x40, "ArchiveWriter size");
