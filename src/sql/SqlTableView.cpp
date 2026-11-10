#include <windows.h>
#include <commctrl.h>
#include "SqlTableView.h"
#include "Node.h"
#include "ida_compat.h"
#include "_imports_p09.h"
#include "_imports_g4.h"

COMDLG_FILTERSPEC g_sqlFileTypes[1] = { { L"SQL File", L"*.sql" } };

SqlTableView::SqlTableView(FileTree* file, Table* table)
    : Document(file),
      m_list(table)
{
}

bool SqlTableView::vfunc_150(int arg)
{
    return m_list.loadRows((const wchar_t*)(INT_PTR)arg);
}

void SqlTableView::onRegisterClass(WNDCLASSEXW* wcx)
{
    wcx->hIcon = (HICON)LoadImageW(hModule, (LPCWSTR)0x8A, IMAGE_ICON, GetSystemMetrics(SM_CXICON),
                                   GetSystemMetrics(SM_CYICON), LR_SHARED);
    wcx->hIconSm = (HICON)LoadImageW(hModule, (LPCWSTR)0x8A, IMAGE_ICON, GetSystemMetrics(SM_CXSMICON),
                                     GetSystemMetrics(SM_CYSMICON), LR_SHARED);
}

bool SqlTableView::onCreate()
{
    if (!Popup::onCreate())
        return false;
    Node* file = FIELD(Node*, this, 0x4C);
    if (file && (file->displayV2() & 2) != 0)
        setModified(true);
    RECT rc;
    RECT* client = getClientRect(&rc);
    return m_list.create(m_hWnd, (HMENU)0x65, client->left, client->top, client->right - client->left,
                         client->bottom - client->top, WS_VISIBLE, 0,
                         client->bottom - client->top);
}

bool SqlTableView::onNotify(NMHDR* hdr, LRESULT* result)
{
    if (hdr->code == (UINT)-181)
        SendMessageW(hdr->hwndFrom, WM_NOTIFY, 0, (LPARAM)hdr);
    return false;
}

COMDLG_FILTERSPEC* SqlTableView::getFilters(int* count)
{
    *count = 1;
    return g_sqlFileTypes;
}
