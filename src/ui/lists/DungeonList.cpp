#include "p05_common.h"
#include "DungeonList.h"
#include "DungeonCoordsViewer.h"
#include "Dungeon.h"
#include "Face.h"
#include "Drawable.h"
#include "Cell.h"
#include "ImageList.h"
#include "Application.h"
#include "String.hpp"

typedef ptrArray<void*> AnyArray;

static LRESULT insertTextItem(HWND tree, LPARAM parent, LPCWSTR text, int image, int mask, int children)
{
    LPARAM ins[16];
    memset(ins, 0, sizeof(ins));
    ins[0] = parent;
    ins[1] = (parent == (LPARAM)TVI_ROOT) ? (LPARAM)TVI_ROOT : (LPARAM)TVI_LAST;
    ins[6] = (LPARAM)text;
    ins[8] = image;
    ins[9] = image;
    ins[2] = mask;
    ins[10] = children;
    return SendMessageW(tree, TVM_INSERTITEMW, 0, (LPARAM)ins);
}

const wchar_t* DungeonList::getNumberFormat()
{
    return L"%.1f";
}

bool DungeonList::onCreate()
{
    bool result = TreeList::onCreate();
    if (!result)
        return result;
    ImageList images;
    images.m_hList = 0;
    images.m_cx = 16;
    images.m_cy = 0;
    images.load(162, 0);
    HIMAGELIST list = images.m_hList;
    images.m_hList = 0;
    SendMessageW(m_tree.m_hWnd, TVM_SETIMAGELIST, TVSIL_NORMAL, (LPARAM)list);
    SendMessageW(m_tree.m_hWnd, TVM_SETIMAGELIST, TVSIL_STATE, (LPARAM)list);
    static const struct { int width; const wchar_t* title; int id; } columns[4] = {
        { 150, L"Name", 'name' }, { 40, L"X", 1 }, { 40, L"Y", 2 }, { 40, L"Z", 3 } };
    for (int i = 0; i < 4; ++i) {
        LVCOLUMNW col;
        memset(&col, 0, 0x2C);
        col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
        col.fmt = m_columnCount ? 0xFFFF : 0xFF0;
        col.cx = columns[i].width;
        col.pszText = (LPWSTR)columns[i].title;
        insertColumn(&col, columns[i].id);
    }
    images.destroy();
    return true;
}

bool DungeonList::onCommand(UINT code, HWND ctrl, UINT id)
{
    return TreeList::onCommand(code, ctrl, id);
}

void DungeonList::onSelChanged(Node* oldNode, Node* newNode)
{
    Drawable* oldDrawable = dynamic_cast<Drawable*>(oldNode);
    Drawable* newDrawable = dynamic_cast<Drawable*>(newNode);
    if (oldDrawable)
        oldDrawable->setFlag(128, false);
    if (newDrawable)
        newDrawable->setFlag(128, true);
    Face* oldFace = dynamic_cast<Face*>(oldNode);
    Face* newFace = dynamic_cast<Face*>(newNode);
    if (oldFace)
        FIELD(BYTE, oldFace, 36) &= ~1u;
    if (newFace)
        FIELD(BYTE, newFace, 36) |= 1u;
}

bool DungeonList::onItemActivate(HTREEITEM item, WPARAM )
{
    LRESULT parent = SendMessageW(m_tree.m_hWnd, TVM_GETNEXTITEM, TVGN_PARENT, (LPARAM)item);
    Cell* cell = getItemCell(item);
    Node* node = cell ? cell->m_node : 0;
    if (!node) {
        String text = getItemText(item);
        if (text.m_length >= 4 && !_wcsnicmp(text.m_buffer + text.m_length - 4, L".tga", 4)) {
            String path;
            path << L"Entity\\Texture\\";
            path.append(text.m_buffer, text.m_length);
            ((Application*)g_studioFrame)->openFile(path.m_buffer);
            return true;
        }
    }
    Face* face = dynamic_cast<Face*>((Displayable*)node);
    if (face) {
        LRESULT grand = SendMessageW(m_tree.m_hWnd, TVM_GETNEXTITEM, TVGN_PARENT, parent);
        Cell* moduleCell = getItemCell((HTREEITEM)grand);
        Node* moduleNode = moduleCell ? moduleCell->m_node : 0;
        Dungeon::Segment::Module* module = dynamic_cast<Dungeon::Segment::Module*>(moduleNode);
        if (module) {
            uint8_t* block = (uint8_t*)((AnyArray*)((uint8_t*)module + 128))->get(0);
            if (block && ((AnyArray*)(block + 164))->contains(face)) {
                AnyArray* vertices = (AnyArray*)(block + 140);
                const unsigned short* idx = (const unsigned short*)face;
                const float* v1 = (const float*)vertices->get(idx[7]);
                const float* v2 = (const float*)vertices->get(idx[8]);
                const float* v3 = (const float*)vertices->get(idx[9]);
                DungeonCoordsViewer dlg(fPoint(v1[2], v1[3], v1[4]), fPoint(v2[2], v2[3], v2[4]),
                                        fPoint(v3[2], v3[3], v3[4]));
                HWND owner = GetParent(GetParent(m_hWnd));
                if (dlg.create(owner)) {
                    dlg.doModal();
                    SetFocus(m_tree.m_hWnd);
                }
            }
        }
    }
    return true;
}

LRESULT DungeonList::setDungeon(Dungeon* dungeon)
{
    LRESULT result = (LRESULT)dungeon;
    m_dungeon = dungeon;
    if (!dungeon)
        return result;
    uint8_t* d = (uint8_t*)dungeon;
    HWND tree = m_tree.m_hWnd;
    LRESULT root = insertTextItem(tree, (LPARAM)TVI_ROOT, L"Segments", 0, 99, 32);
    AnyArray* segments = (AnyArray*)(d + 176);
    int segmentCount = segments->size();
    result = segmentCount;
    for (int s = 0; s < segmentCount; ++s) {
        uint8_t* segment = (uint8_t*)segments->get(s);
        LPARAM segmentItem;
        if (!segment || segment == (uint8_t*)-104) {
            segmentItem = 0;
        } else {
            Cell* c = new Cell((Node*)(segment + 104));
            segmentItem = (LPARAM)insertCell(c, (HTREEITEM)root, 2, 0, TVI_LAST);
        }
        AnyArray* modules = (AnyArray*)(segment + 200);
        int moduleCount = modules->size();
        for (int m = 0; m < moduleCount; ++m) {
            uint8_t* module = (uint8_t*)modules->get(m);
            LPARAM moduleItem;
            if (!module || module == (uint8_t*)-104) {
                moduleItem = 0;
            } else {
                Cell* c = new Cell((Node*)(module + 104));
                moduleItem = (LPARAM)insertCell(c, (HTREEITEM)segmentItem, 14, 0, TVI_LAST);
            }
            uint8_t* texture = (uint8_t*)((AnyArray*)(d + 124))->get(FIELD(int, module, 120));
            if (texture)
                insertTextItem(tree, moduleItem, FIELD(LPCWSTR, texture, 24), 9, 35, 0);
            AnyArray* blocks = (AnyArray*)(module + 128);
            int blockCount = blocks->size();
            if (blockCount == 1) {
                uint8_t* block = (uint8_t*)blocks->get(0);
                if (block) {
                    AnyArray* faces = (AnyArray*)(block + 164);
                    int faceCount = faces->size();
                    if (faceCount > 0) {
                        LRESULT blocksItem = insertTextItem(tree, moduleItem, L"Blocks", 3, 99, faceCount);
                        for (int f = 0; f < faceCount; ++f) {
                            Node* face = (Node*)faces->get(f);
                            if (face) {
                                Cell* c = new Cell(face);
                                insertCell(c, (HTREEITEM)blocksItem, 4, 0, TVI_LAST);
                            }
                        }
                    }
                }
            } else if (blockCount > 1) {
                insertTextItem(tree, moduleItem, L"More than 1 Block -- unhandled", 5, 35, 0);
            }
        }
        AnyArray* bounds = (AnyArray*)(segment + 224);
        result = bounds->size();
        if (result > 0) {
            uint8_t* first = (uint8_t*)bounds->get(0);
            result = (LRESULT)first;
            if (first) {
                AnyArray* items = (AnyArray*)(first + 24);
                int count = items->size();
                result = count;
                if (count > 0) {
                    LRESULT boundsItem = insertTextItem(tree, segmentItem, L"Bounds", 12, 99, count);
                    result = boundsItem;
                    for (int b = 0; b < count; ++b) {
                        Node* bound = (Node*)items->get(b);
                        result = (LRESULT)bound;
                        if (bound) {
                            Cell* c = new Cell(bound);
                            result = (LRESULT)insertCell(c, (HTREEITEM)boundsItem, 13, 0, TVI_LAST);
                        }
                    }
                }
            }
        }
    }
    return result;
}
