#include "p05_common.h"
#include "MlxView.h"
#include "ModelsMLX.h"

static inline const String* temporaryAddress(const String& s) { return &s; }

MlxView::MlxView(FileTree* node, ModelsMLX* models, void* items)
    : ModelView(node, models ? (Serializable*)((uint8_t*)models + 24) : 0, &m_list),
      m_models(models),
      m_list(this, models, temporaryAddress(getDirectory())),
      m_gears{ ListGears(items), ListGears(items), ListGears(items), ListGears(items) },
      m_name(), m_3dc(), m_dds(), m_alpha(), m_btnUpdate(), m_btnAppend(), m_btnRename()
{
    m_bkColor = panelBackgroundColor();
    memset(m_raceCode, 0, sizeof(m_raceCode));
}

MlxView::~MlxView()
{
}

LPCWSTR MlxView::getClassName()
{
    return L"ModelXView";
}

int MlxView::getMenuId()
{
    return 170;
}

void MlxView::onRegisterClass(WNDCLASSEXW* wc)
{
    int cy = GetSystemMetrics(SM_CYICON);
    int cx = GetSystemMetrics(SM_CXICON);
    wc->hIcon = (HICON)LoadImageW(hModule, (LPCWSTR)0x87, IMAGE_ICON, cx, cy, LR_SHARED);
    int cySm = GetSystemMetrics(SM_CYSMICON);
    int cxSm = GetSystemMetrics(SM_CXSMICON);
    wc->hIconSm = (HICON)LoadImageW(hModule, (LPCWSTR)0x87, IMAGE_ICON, cxSm, cySm, LR_SHARED);
}

SIZE* MlxView::getDefaultSize(SIZE* out, SIZE )
{
    out->cx = 800;
    out->cy = 680;
    return out;
}

bool MlxView::vfunc_FC(int msg)
{
    return IsDialogMessageW(m_hWnd, (LPMSG)msg) != 0;
}
