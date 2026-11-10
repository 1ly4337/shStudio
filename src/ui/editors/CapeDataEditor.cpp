#include "CapeDataEditor.h"
#include "String.hpp"
#include "iStream.h"
#include "oStream.h"
#include "Clipboard.h"
#include "g2_inline.h"
#include "_imports_p07.h"

static inline CapeData::Block* capeBlocks(CapeData* cape)
{
    return &FIELD(CapeData::Block, cape, 0x3C);
}

static inline void copyBlock(CapeData::Block& dst, const CapeData::Block& src)
{
    FIELD(DWORD, &dst, 0x0C) = FIELD(DWORD, &src, 0x0C);
    for (int k = 0; k < 3; k++)
    {
        FIELD(DWORD, &dst, 0x10 + 4 * k) = FIELD(DWORD, &src, 0x10 + 4 * k);
        FIELD(DWORD, &dst, 0x1C + 4 * k) = FIELD(DWORD, &src, 0x1C + 4 * k);
    }
}

CapeDataEditor::BlockList::BlockList(CapeData* cape)
    : m_cape(cape), m_list(256)
{
    for (int i = 0; i < 20; i++)
    {
        copyBlock(m_blocks[i], capeBlocks(m_cape)[i]);
        m_list.add(&m_blocks[i]);
    }
}

CapeDataEditor::BlockList::~BlockList()
{
    for (int i = 0; i < m_list.m_count; i++)
        m_list.m_items[i] = 0;
    m_list.m_count = 0;
}

void CapeDataEditor::BlockList::save()
{
    for (int i = 0; i < 20; i++)
        copyBlock(capeBlocks(m_cape)[i], m_blocks[i]);
}

bool CapeDataEditor::BlockList::onCreate()
{
    if (!ListCtrlOD::onCreate())
        return false;
    addColumn(L"Indx", 40, 18, 0);
    addColumn(L"Pt 1", 40, 18, 1);
    addColumn(L"Pt 2", 40, 18, 2);
    addColumn(L"Pt 3", 40, 18, 3);
    addColumn(L"Fl 1", 70, 130, 4);
    addColumn(L"Fl 2", 70, 130, 5);
    addColumn(L"Fl 3", 70, 130, 6);
    return true;
}

Node* CapeDataEditor::BlockList::getNode(int index)
{
    return m_list.get(index);
}

bool CapeDataEditor::BlockList::copy()
{
    oStream out(false, 1200);
    out << (unsigned long)1;
    m_cape->write(out, 'clip');
    bool ok;
    if (Clipboard::setData(0x22E, out, true))
    {
        String text;
        m_cape->toText(text, 'dump');
        if (text.m_length)
            ok = Clipboard::setText(text.m_buffer, false);
        else
            ok = true;
    }
    else
    {
        ok = false;
    }
    return ok;
}

bool CapeDataEditor::BlockList::paste()
{
    iStream in(false, 1200);
    if (!Clipboard::getData(0x22E, in) || in.readInt() != 1)
        return false;

    CapeData cape(0);
    cape.read(in, 'clip');
    for (int i = 0; i < 20; i++)
        copyBlock(m_blocks[i], capeBlocks(&cape)[i]);
    invalidate(true);
    return true;
}

CapeDataEditor::CapeDataEditor(CapeData* cape)
    : m_cape(cape), m_blockList(cape)
{
}

CapeDataEditor::~CapeDataEditor()
{
}

bool CapeDataEditor::create(HWND parent)
{
    m_buttons = MB_OKCANCEL;
    RECT rc;
    SetRect(&rc, (int)0x80000000, (int)0x80000000, 450, 490);
    bool result = Window::create(parent, 0, &rc, 0x80C80000, 0x10101, L"Cape Editor");
    if (result)
    {
        m_owner = Modal_getTopLevelWindow(parent);
        Window_centerOn(m_hWnd, m_owner);
        show(true);
        return true;
    }
    return result;
}

bool CapeDataEditor::onCreate()
{
    g2_createStatic(m_hWnd, L"3DC:", 20, 17, 30, 14, 4);
    m_edit3DC.createNumber(m_hWnd,(HMENU) (HMENU)100, 50, 15, 60, 18, FIELD(int, m_cape, 0x14) , 0, 0);
    g2_createStatic(m_hWnd, L"(index of 3DC file as listed in the Capes panel)", 120, 17, 260, 14, 4);
    g2_createStatic(m_hWnd, L"DDS:", 20, 41, 30, 14, 4);
    m_editDDS.createNumber(m_hWnd,(HMENU) (HMENU)101, 50, 39, 60, 18, FIELD(int, m_cape, 0x10) , 0, 0);
    g2_createStatic(m_hWnd, L"(index of DDS file in the .CTL file for this class)", 120, 41, 260, 14, 4);
    g2_createStatic(m_hWnd, L"The meaning of other parameters is (currently) unknown:", 20, 65, 350, 14, 4);

    g2_createStatic(m_hWnd, L"#1:", 20, 87, 30, 14, 4);
    m_edits[0].createNumber(m_hWnd,(HMENU) (HMENU)102, 50, 85, 60, 18, FIELD(int, m_cape, 0x18), 0, 0);
    g2_createStatic(m_hWnd, L"#2:", 120, 87, 30, 14, 4);
    m_edits[1].createNumber(m_hWnd,(HMENU) (HMENU)103, 150, 85, 60, 18, FIELD(int, m_cape, 0x20), 0, 0);
    g2_createStatic(m_hWnd, L"#3:", 220, 87, 30, 14, 4);
    m_edits[2].createNumber(m_hWnd,(HMENU) (HMENU)104, 250, 85, 60, 18, FIELD(int, m_cape, 0x24), 0, 0);

    g2_createStatic(m_hWnd, L"#4a:", 20, 111, 30, 14, 4);
    m_edits[3].createNumber(m_hWnd,(HMENU) (HMENU)105, 50, 109, 60, 18, FIELD(int, m_cape, 0x28), 0, 0);
    g2_createStatic(m_hWnd, L"#4b:", 120, 111, 30, 14, 4);
    m_edits[4].createNumber(m_hWnd,(HMENU) (HMENU)106, 150, 109, 60, 18, FIELD(int, m_cape, 0x2C), 0, 0);
    g2_createStatic(m_hWnd, L"#4c:", 220, 111, 30, 14, 4);
    m_edits[5].createNumber(m_hWnd,(HMENU) (HMENU)107, 250, 109, 60, 18, FIELD(int, m_cape, 0x30), 0, 0);
    g2_createStatic(m_hWnd, L"#4d:", 320, 111, 30, 14, 4);
    m_edits[6].createNumber(m_hWnd,(HMENU) (HMENU)108, 350, 109, 60, 18, FIELD(int, m_cape, 0x34), 0, 0);

    g2_createStatic(m_hWnd, L"??:", 20, 135, 30, 14, 4);
    m_edits[7].createNumber(m_hWnd,(HMENU) (HMENU)109, 50, 133, 60, 18, FIELD(int, m_cape, 0x1C), 0, 0);
    g2_createStatic(m_hWnd, L"(apparently, duplicated .3DC index)", 120, 135, 260, 14, 4);

    g2_createStatic(m_hWnd, L"Cape data", 20, 167, 260, 14, 4);
    m_blockList.create(m_hWnd, (HMENU)0x6E, 20, 185, 400, 200, WS_BORDER, 0, 0);
    m_blockList.setItemCount(20);
    g2_createStatic(m_hWnd,
                    L"(use Ctrl+C to copy the full list, then Ctrl+V to paste it in a newly created record)",
                    20, 389, 400, 14, 4);
    return Modal::onCreate();
}

int CapeDataEditor::mapCommandToResult(int id)
{
    if (id == IDOK)
    {
        FIELD(int, m_cape, 0x14) = m_edit3DC.getInt(-1);
        FIELD(int, m_cape, 0x10) = m_editDDS.getInt(-1);
        FIELD(int, m_cape, 0x18) = m_edits[0].getInt(-1);
        FIELD(int, m_cape, 0x20) = m_edits[1].getInt(-1);
        FIELD(int, m_cape, 0x24) = m_edits[2].getInt(-1);
        FIELD(int, m_cape, 0x28) = m_edits[3].getInt(-1);
        FIELD(int, m_cape, 0x2C) = m_edits[4].getInt(-1);
        FIELD(int, m_cape, 0x30) = m_edits[5].getInt(-1);
        FIELD(int, m_cape, 0x34) = m_edits[6].getInt(-1);
        FIELD(int, m_cape, 0x1C) = m_edits[7].getInt(-1);
        m_blockList.save();
    }
    return id;
}
