#pragma once
#include "ContentProvider.h"
#include "String.hpp"

class DataProvider : public ContentProvider
{
public:
    DataProvider(FolderRoot* root, int format) : m_root(root), m_format(format) {}
    virtual ~DataProvider() {}

    virtual int open(HWND parent, FolderRoot* root) = 0;
    virtual int readRaw(FileTree* file, void* buffer, DWORD* size) = 0;

    FolderRoot* m_root;
    int         m_format;
    String      m_directory;
};
static_assert(sizeof(DataProvider) == 0x2C, "DataProvider size");
