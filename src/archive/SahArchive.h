#pragma once
#include <windows.h>
#include <cstring>
#include "String.hpp"
#include "Bytes.h"

class FolderRoot;
class FolderTree;
class FileTree;
class StreamNotifier;

#pragma pack(push, 1)
struct SahHeader
{
    SahHeader() : version(0), fileCount(0)
    {
        magic[0] = 'S';
        magic[1] = 'A';
        magic[2] = 'H';
        memset(reserved, 0, sizeof(reserved));
    }

    char magic[3];
    int  version;
    int  fileCount;
    char reserved[40];
};
#pragma pack(pop)
static_assert(sizeof(SahHeader) == 51, "SahHeader size");

void decodeSahIndex(Bytes& in, Bytes& out);

class SahArchive
{
public:
    class Holes
    {
    public:
#pragma pack(push, 4)
        struct Hole
        {
            __int64 offset;
            int     size;
        };
#pragma pack(pop)

        Holes() : m_capacity(0), m_count(0), m_items(0) {}
        ~Holes()
        {
            if (m_items)
                delete[] m_items;
            m_items = 0;
        }

        void add(__int64 offset, int size);
        void consume(__int64 offset, int size);
        __int64 findBest(int size);
        int read(iStream& in);

        int   m_capacity;
        int   m_count;
        Hole* m_items;
    };

    SahArchive(FolderRoot* root, int codePage);
    ~SahArchive();

    void close();
    DWORD open(LPCWSTR safPath);
    int readIndex(StreamNotifier* notifier);
    int readFile(FileTree* file, iStream& out);
    int readRaw(FileTree* file, void* buffer, DWORD* size);
    static int writeAt(HANDLE file, const LARGE_INTEGER* position, const void* data, DWORD size);
    int writeFile(FileTree* file, oStream& data);
    FileTree* addFile(FolderTree* folder, WCHAR* path, int* error);
    int saveIndex();
    int saveHoles();

    FolderRoot*   m_root;
    Holes         m_holes;
    int           m_codePage;
    HANDLE        m_file;
    LARGE_INTEGER m_fileSize;
    int           m_fileCount;
    Bytes         m_header;
    String        m_sahPath;
    String        m_safPath;
    bool          m_writable;
};
static_assert(sizeof(SahArchive) == 0x98, "SahArchive size");
