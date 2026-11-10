#pragma once
#include "DataProvider.h"

class DataFileProvider : public DataProvider
{
public:
    DataFileProvider(FolderRoot* root, int format);
    virtual ~DataFileProvider();

    virtual bool isArchive() override { return false; }
    virtual const wchar_t* getFolder() override { return m_directory.m_buffer; }
    virtual const wchar_t* getDirectory() override { return m_directory.m_buffer; }
    virtual int readFile(FileTree* file, iStream* out, bool decrypt) override;
    virtual int readPath(const wchar_t* path, iStream* out) override;
    virtual int writeFile(FileTree* file, aStream* in, bool encrypt) override;
    virtual int open(HWND parent, FolderRoot* root) override;
    virtual int readRaw(FileTree* file, void* buffer, DWORD* size) override;
};
static_assert(sizeof(DataFileProvider) == 0x2C, "DataFileProvider size");
