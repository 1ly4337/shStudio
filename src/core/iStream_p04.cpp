#include <windows.h>
#include "iStream.hpp"
#include "FileTree.h"

#pragma warning(disable : 4066)

FileTree* iStream::readFileTree()
{
    FileTree* file = new FileTree();
    try
    {
        file->read(*this, 'file');
    }
    catch (...)
    {
        delete file;
        return 0;
    }
    return file;
}
