#include "AltView.h"
#include "CharActions.h"
#include "_imports_p06.h"

#include "Application.h"

AltView::AltView(FileTree* file, CharActions* actions)
    : DocView(file, actions)
    , m_list(this)
{
    m_searchEnabled = false;
    m_decrypt = false;
    if (actions)
    {
        m_actions = actions;
    }
    else
    {
        m_actions = new CharActions();
        m_content = m_actions;
        FIELD(Serializable*, this, 0x50) = m_actions;
    }
    m_list.m_data = m_actions;
}

AltView::~AltView()
{
}

LPCWSTR AltView::getClassName()
{
    return L"AltView";
}

void AltView::onRegisterClass(WNDCLASSEXW* wcx)
{
    wcx->hIcon = (HICON)LoadImageW(hModule, MAKEINTRESOURCEW(0x85), IMAGE_ICON,
                                   GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), LR_SHARED);
    wcx->hIconSm = (HICON)LoadImageW(hModule, MAKEINTRESOURCEW(0x85), IMAGE_ICON,
                                     GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_SHARED);
}

bool AltView::onCreate()
{
    if (!DocView::onCreate())
        return false;
    HWND parent = m_hWnd;
    RECT rcBuf;
    RECT* rc = getClientRect(&rcBuf);
    int height = rc->bottom - rc->top;
    return m_list.create(parent, (HMENU)0x65, rc->left, rc->top, rc->right - rc->left, height,
                             0x50000000, 0, height) != 0;
}

void AltView::onActivate(bool active)
{
    m_list.onActivate(active);
}

int AltView::getMenuId()
{
    return 166;
}
