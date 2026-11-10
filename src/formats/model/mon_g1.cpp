#include "mon_g1.h"
#include "Bytes.h"
#include "Model3D.h"
#include "_imports_p07.h"

const wchar_t* monModelVersion(const wchar_t* fileName)
{
    Bytes file;
    if (file.aStream::load(fileName))
        return L"--";
    Model3D model('k3DC');
    model.read(file, 'file');
    switch (model.m_version & 0xFFF)
    {
    case 0x600: return L"6";
    case 0x400: return L"4";
    case 0x500: return L"5";
    case 0x620: return L"6.2";
    default:    return L"--";
    }
}
