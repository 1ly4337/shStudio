#include "p05_common.h"
#include "WorkspaceDataSet.h"
#include "DataProvider.h"
#include "Application.h"

WorkspaceDataSet::~WorkspaceDataSet()
{
}

const wchar_t* WorkspaceDataSet::getString(DWORD tag) const
{
    if (tag != 'name')
        return 0;
    DataProvider* src = m_source;
    if (src)
        return src->getDirectory();
    return L"Client";
}

void WorkspaceDataSet::onCommand(int cmd, int )
{
    g_mainFrame->postCommand(cmd);
}
