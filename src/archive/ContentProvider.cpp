#include "ContentProvider.h"
#include "DataFileProvider.h"
#include "ArchiveProvider.h"
#include "String.hpp"
#include "_imports_p09.h"
#include "_imports_g2.h"

ContentProvider* g_contentProvider = 0;

DataFileProvider::DataFileProvider(FolderRoot* root, int format)
    : DataProvider(root, format)
{
}

DataFileProvider::~DataFileProvider()
{
}

ArchiveProvider::ArchiveProvider(FolderRoot* root, int format)
    : DataProvider(root, format), m_archive(root, format)
{
}

ArchiveProvider::~ArchiveProvider()
{
}

const wchar_t* ArchiveProvider::getFolder()
{
    return L"";
}

String getContentFolder()
{
    if (g_contentProvider)
    {
        const wchar_t* folder = g_contentProvider->getFolder();
        String result;
        result.append(folder, (unsigned)-1);
        return result;
    }
    return String();
}

String getContentDirectory()
{
    String result;
    if (g_contentProvider)
        result = g_contentProvider->getDirectory();
    return result;
}
