#include "Serializable.h"
#include "String.hpp"
#include "iStream.hpp"
#include <cstdlib>
#include <cwchar>

Serializable::~Serializable()
{
    if (m_buffer1)
    {
        ::operator delete[](m_buffer1);
        m_buffer1 = 0;
    }
    if (m_buffer2)
    {
        ::operator delete[](m_buffer2);
        m_buffer2 = 0;
    }
    if (m_fileName)
    {
        free(m_fileName);
        m_fileName = 0;
    }
}

void Serializable::setFileName(const wchar_t* fileName)
{
    if (m_fileName)
    {
        free(m_fileName);
        m_fileName = 0;
    }
    if (fileName)
        m_fileName = _wcsdup(fileName);
}

int Serializable::getCodePage() const
{
    if (!m_fileName)
        return -1;
    String name(m_fileName, (unsigned)-1);
    name.removeExtension();
    int codePage;
    if (name.m_length >= 3 && !_wcsnicmp(name.m_buffer + (name.m_length - 3), L"CHN", 3))
        codePage = 936;
    else if (name.endsWithNoCase(L"KOR"))
        codePage = 949;
    else if (name.endsWithNoCase(L"FRC") || name.endsWithNoCase(L"GER") || name.endsWithNoCase(L"ITA")
             || name.endsWithNoCase(L"USA"))
        codePage = 1252;
    else if (name.endsWithNoCase(L"POL") || name.endsWithNoCase(L"TUR"))
        codePage = 1250;
    else
        codePage = -1;
    return codePage;
}

void Serializable::checkSignature(iStream& in, const wchar_t* expected)
{
    String signature;
    signature.read(in, 1, 1200);
    if (!signature.equalsNoCase(expected))
        throw (const wchar_t*)L"Invalid or not supported DB format";
}
