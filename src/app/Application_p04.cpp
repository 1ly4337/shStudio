#include <windows.h>
#include "Application.h"
#include "String.hpp"
#include "_imports_p04.h"

Application::~Application()
{
}

String* Application::getWorkspacePath(String* out)
{
    *out = L"";
    return out;
}

LPCWSTR Application::getClassName()
{
    return L"Application";
}

DWORD Application::getExStyle()
{
    return WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
}

Document* Application::createView(String* ext, FileTree* node, bool reload)
{
    return 0;
}
