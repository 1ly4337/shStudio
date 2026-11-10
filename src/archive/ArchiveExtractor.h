#pragma once
#include <windows.h>
#include "String.hpp"
#include "Bytes.h"

class FolderTree;
class FileTree;
class StreamNotifier;

class ArchiveExtractor
{
public:
    ArchiveExtractor();
    ~ArchiveExtractor();

    DWORD open(const wchar_t* safPath);
    int extract(FolderTree* root, const wchar_t* destination, StreamNotifier* notifier);
    int extractFolder(FolderTree* folder, const String& parentDir, StreamNotifier* notifier);
    int extractFile(FileTree* file, const String& dir);

    bool          m_fromTree;
    BYTE*         m_buffer;
    HANDLE        m_file;
    int           m_unused0C;
    LARGE_INTEGER m_fileSize;
    String        m_destination;
    Bytes         m_index;
    bool          m_cancelled;
    int           m_unused6C;
    int           m_extracted;
    int           m_unused74;
    String        m_errors;
};
static_assert(sizeof(ArchiveExtractor) == 0x98, "ArchiveExtractor size");
