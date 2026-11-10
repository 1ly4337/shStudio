#include <windows.h>
#include <commctrl.h>
#include "FolderTreeFill.h"
#include "TreeList.h"
#include "FolderTree.h"
#include "FileTree.h"
#include "Cell.h"
#include "ptrArray.h"
#include "String.hpp"
#include "ContentFiles.h"
#include "ida_compat.h"
#include "_imports_p09.h"
#include "_imports_g4.h"

void __fastcall fillFolderTree(TreeList* tree, FolderTree* folder)
{
    LPARAM parent = folder->m_data;
    ptrArray<FolderTree*>& folders = FIELD(ptrArray<FolderTree*>, folder, 0x4C);
    int folderCount = folders.size();
    for (int i = 0; i < folderCount; ++i)
    {
        FolderTree* child = folders.get(i);
        if (child)
        {
            tree->TreeList::insertCell(new Cell(child), (HTREEITEM)parent, 1, -1, TVI_LAST);
            fillFolderTree(tree, child);
        }
    }

    ptrArray<FileTree*>& files = FIELD(ptrArray<FileTree*>, folder, 0x64);
    int fileCount = files.size();
    for (int i = 0; i < fileCount; ++i)
    {
        FileTree* file = files.get(i);
        if (file)
        {
            int image = getContentFileType(FIELD(String, file, 0x10).m_buffer);
            if (image >= 0)
            {
                FIELD(int, file, 0xF8) = image;
                tree->TreeList::insertCell(new Cell(file), (HTREEITEM)parent, image, 0, TVI_LAST);
            }
        }
    }
}
