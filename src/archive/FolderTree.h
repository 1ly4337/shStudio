#pragma once
#include <windows.h>
#include <cstddef>
#include "Node.h"
#include "String.hpp"
#include "ptrArray.h"
#include "FileTree.h"

class iStream;
class oStream;
class StreamNotifier;

class NodeComparator
{
public:
    virtual int compare(const Node* a, const Node* b) = 0;
};

#pragma warning(disable : 4250)
#pragma vtordisp(push, 2)

class FolderTree : public Node
{
public:
    class FoldersTree : public ptrArray<FolderTree*>
    {
    public:
        FoldersTree() {}
        virtual ~FoldersTree() { deleteAll(); }
        void sort(int lo, int hi, NodeComparator* cmp);
    };

    class FilesTree : public ptrArray<FileTree*>
    {
    public:
        FilesTree() {}
        virtual ~FilesTree() { deleteAll(); }
        void sort(int lo, int hi, NodeComparator* cmp);
    };

    FolderTree();
    FolderTree(const wchar_t* name, const wchar_t* path, int codePage);
    virtual ~FolderTree();

    virtual int getId() const override { return 0; }
    virtual const wchar_t* getString(DWORD tag) const override;
    virtual int getInt(DWORD tag) const override;
    virtual void reset() override;
    virtual int compare(const Node* other, DWORD tag) const override;
    virtual DWORD scan(StreamNotifier* notifier);
    virtual FileTree* findFile(const wchar_t* name);
    virtual FileTree* createFile();
    virtual FolderTree* createFolder();
    virtual int compareName(const FolderTree* other) const;

    virtual void read(iStream& in, DWORD fmt) override;
    virtual void write(oStream& out, DWORD fmt) const override;
    virtual bool toText(String& out, DWORD fmt) const override;

    int countFiles() const;
    void readTree(iStream& in, DWORD fmt, StreamNotifier* notifier);
    void writeTree(oStream& out, DWORD fmt, StreamNotifier* notifier) const;
    FolderTree* findChildFolder(const wchar_t* name) const;
    FileTree* findChildFile(const wchar_t* name) const;
    FileTree* addFile(const wchar_t* name, __int64 size);
    FolderTree* addFolder(const wchar_t* name, const wchar_t* parentPath);
    DWORD addFindData(const WIN32_FIND_DATAW* data, StreamNotifier* notifier);
    __int64 computeSize();

public:
    String      m_path;
    String      m_name;
    FoldersTree m_folders;
    FilesTree   m_files;
    int         m_pad7C;
    __int64     m_totalSize;
    int         m_codePage;
    int         m_pad8C;
};

#pragma vtordisp(pop)

static_assert(sizeof(FolderTree::FoldersTree) == 0x18, "FoldersTree size");
static_assert(sizeof(FolderTree::FilesTree) == 0x18, "FilesTree size");
static_assert(offsetof(FolderTree, m_path) == 0x0C, "FolderTree::m_path");
static_assert(offsetof(FolderTree, m_name) == 0x2C, "FolderTree::m_name");
static_assert(offsetof(FolderTree, m_folders) == 0x4C, "FolderTree::m_folders");
static_assert(offsetof(FolderTree, m_files) == 0x64, "FolderTree::m_files");
static_assert(offsetof(FolderTree, m_totalSize) == 0x80, "FolderTree::m_totalSize");
static_assert(offsetof(FolderTree, m_codePage) == 0x88, "FolderTree::m_codePage");
static_assert(sizeof(FolderTree) == 0x98, "FolderTree size");
