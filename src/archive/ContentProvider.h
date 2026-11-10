#pragma once
#include <windows.h>

class aStream;
class iStream;
class FileTree;
class FolderTree;
class FolderRoot;

class ContentProvider
{
public:
    ContentProvider() {}
    virtual ~ContentProvider() {}
    virtual bool isArchive() = 0;
    virtual const wchar_t* getFolder() = 0;
    virtual const wchar_t* getDirectory() = 0;
    virtual int readFile(FileTree* file, iStream* out, bool decrypt) = 0;
    virtual int readPath(const wchar_t* path, iStream* out) = 0;
    virtual int writeFile(FileTree* file, aStream* in, bool encrypt) = 0;
};
static_assert(sizeof(ContentProvider) == 0x4, "ContentProvider size");

extern ContentProvider* g_contentProvider;

class String;
String getContentFolder();
String getContentDirectory();
