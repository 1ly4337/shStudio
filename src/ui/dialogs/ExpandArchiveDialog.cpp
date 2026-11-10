#include "ExpandArchiveDialog.h"
#include "ArchiveExtractor.h"
#include "_imports_p09.h"
#include "_imports_g2.h"

ExpandArchiveDialog::ExpandArchiveDialog(ArchiveExtractor* extractor, FolderRoot* root, const wchar_t* workspace)
    : ArchiveDialog(root, true),
      m_extractor(extractor)
{
    m_workspace.append(workspace, (unsigned)-1);
}

ExpandArchiveDialog::~ExpandArchiveDialog()
{
}

LPCWSTR ExpandArchiveDialog::getTitle()
{
    return L"Creating Workspace";
}

void* ExpandArchiveDialog::getWorkerProc()
{
    return (void*)threadProc;
}

__int64 ExpandArchiveDialog::getWorkSize()
{
    return (int)(m_extractor->m_fileSize.QuadPart >> 16);
}
