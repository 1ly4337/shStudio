#pragma once
#include <windows.h>

class aStream;
class iStream;
class Bytes;
class FileTree;
class FolderRoot;
class String;

int loadDataFile(FileTree* file, iStream* out, bool decrypt);
int encryptDataFile(aStream* in, BYTE* key, Bytes& out);
int getContentFileType(wchar_t* fileName);
int addArchiveFile(FolderRoot* root, String& dir, wchar_t* name, int codePage);
