#pragma once
#include <windows.h>
#include "ida_compat.h"
#include "String.hpp"
#include "ptrArray.h"
#include "FolderTree.h"
#include "FileTree.h"

inline String& FolderTree_path(FolderTree* f)   { return FIELD(String, f, 0x0C); }
inline String& FolderTree_name(FolderTree* f)   { return FIELD(String, f, 0x2C); }
inline ptrArray<FolderTree*>& FolderTree_folders(FolderTree* f) { return FIELD(ptrArray<FolderTree*>, f, 0x4C); }
inline ptrArray<FileTree*>&   FolderTree_files(FolderTree* f)   { return FIELD(ptrArray<FileTree*>, f, 0x64); }
inline __int64& FolderTree_totalSize(FolderTree* f) { return FIELD(__int64, f, 0x80); }
inline int& FolderTree_context(FolderTree* f)   { return FIELD(int, f, 0x88); }

inline String& FileTree_path(FileTree* f)       { return FIELD(String, f, 0x10); }
inline String& FileTree_name(FileTree* f)       { return FIELD(String, f, 0x30); }
inline __int64& FileTree_offset(FileTree* f)    { return FIELD(__int64, f, 0x58); }
inline __int64& FileTree_size(FileTree* f)      { return FIELD(__int64, f, 0x60); }
inline int& FileTree_type(FileTree* f)          { return FIELD(int, f, 0xF8); }
inline BYTE* FileTree_key(FileTree* f)          { return &FIELD(BYTE, f, 0x100); }
