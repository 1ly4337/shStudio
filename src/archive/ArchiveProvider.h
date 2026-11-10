#pragma once
#include "DataProvider.h"
#include "SahArchive.h"

class Window;

class ArchiveProvider : public DataProvider
{
public:
    ArchiveProvider(FolderRoot* root, int format);
    virtual ~ArchiveProvider();

    virtual bool isArchive() override { return true; }
    virtual const wchar_t* getFolder() override;
    virtual const wchar_t* getDirectory() override { return m_directory.m_buffer; }
    virtual int readFile(FileTree* file, iStream* out, bool decrypt) override;
    virtual int readPath(const wchar_t* path, iStream* out) override;
    virtual int writeFile(FileTree* file, aStream* in, bool encrypt) override;
    virtual int open(HWND parent, FolderRoot* root) override;
    virtual int readRaw(FileTree* file, void* buffer, DWORD* size) override;
    virtual int addFile(Window* tree, FolderTree* folder, wchar_t* fileName);

    SahArchive m_archive;
};
static_assert(sizeof(ArchiveProvider) == 0xC8, "ArchiveProvider size");
