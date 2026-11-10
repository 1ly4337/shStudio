#include "fileutil.h"
#include "String.hpp"
#include <shlwapi.h>

bool fileExistsIn(const wchar_t* dir, const wchar_t* name)
{
    String path(dir, (unsigned)-1);
    path << L'\\';
    path << name;
    return PathFileExistsW(path.m_buffer) != 0;
}
