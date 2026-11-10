#include "EffectView.h"
#include "EffectSets.h"
#include "Application.h"
#include "_imports_p06.h"

EffectView::EffectView(FileTree* file, void* )
    : DocView(file, nullptr)
    , m_list(this)
{
    EffectSets* sets = new EffectSets();
    m_sets = sets;
    m_list.m_data = sets;
    m_content = sets;
    FIELD(Serializable*, this, 0x50) = sets;
}

EffectView::~EffectView()
{
}

LPCWSTR EffectView::getClassName()
{
    return L"EffectView";
}

void EffectView::onRegisterClass(WNDCLASSEXW* wcx)
{
    wcx->hIcon = (HICON)LoadImageW(hModule, MAKEINTRESOURCEW(0x86), IMAGE_ICON,
                                   GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), LR_SHARED);
    wcx->hIconSm = (HICON)LoadImageW(hModule, MAKEINTRESOURCEW(0x86), IMAGE_ICON,
                                     GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_SHARED);
}

bool EffectView::onCreate()
{
    if (!DocView::onCreate())
        return false;
    HWND parent = m_hWnd;
    RECT rcBuf;
    RECT* rc = getClientRect(&rcBuf);
    int height = rc->bottom - rc->top;
    if (!m_list.create(parent, (HMENU)0x65, rc->left, rc->top, rc->right - rc->left, height,
                           0x50000000, 0, height))
        return false;
    m_list.setItemCount(m_sets->size());
    return true;
}

bool EffectView::onCommand(UINT code, HWND ctrl, UINT id)
{
    return DocView::onCommand(code, ctrl, id);
}

bool EffectView::onInitMenu(HMENU menu)
{
    setStatusText(0, 0);
    HWND list = GetDlgItem(m_hWnd, 0x65);
    if (list)
        SendMessageW(list, WM_INITMENU, (WPARAM)menu, 0);
    enableMenuItem(0xE124, IsClipboardFormatAvailable(0x206) == 1);
    return true;
}

int EffectView::getMenuId()
{
    return 0xAA;
}

EffectSet* EffectView::getSet(int index)
{
    return m_sets->get(index);
}
