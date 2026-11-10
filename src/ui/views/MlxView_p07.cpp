#include "MlxView.h"
#include "MlxList.h"
#include "ListGears.h"
#include "ModelMLX.h"
#include "ModelsMLX.h"
#include "ModelMLT.h"
#include "ModelsMLT.h"
#include "MetaBody.h"
#include "PopupView3D.h"
#include "ThreeDxView.h"
#include "ThreeDxCtrlsPanel.h"
#include "StudioFrame.h"
#include "FolderRoot.h"
#include "ContentProvider.h"
#include "EditText.h"
#include "ComboBox.h"
#include "Button.h"
#include "Strings.h"
#include "Bytes.h"
#include "String.hpp"
#include "primitiveVector.h"
#include "mlx_g1.h"
#include "_imports_p07.h"
#include <commctrl.h>

#pragma warning(disable : 4066 4498)

#define DOCVIEW_DOC(v)       FIELD(BYTE*, (v), 0x4C)
#define DOCVIEW_BAR(v)       FIELD(HWND, (v), 0x38)
#define DOCVIEW_BAR_H(v)     FIELD(int, (v), 0x40)
#define DOC_PATH(d)          FIELD(String, (d), 0x10)
#define DOC_FILENAME(d)      FIELD(const wchar_t*, (d), 0x34)
#define FRAME_FOLDERS()      FIELD(FolderRoot, g_studioFrame, 0x178)
#define PANEL_TEXTURE(p)     FIELD(EditText, (p), 0x208)

static inline HWND mlxCreateLabel(HWND parent, const wchar_t* text, DWORD style, int x, int y, int cx, int cy)
{
    HWND label = CreateWindowExW(0, L"Static", text, style, x, y, cx, cy, parent, 0, 0, 0);
    if (label)
        SendMessageW(label, WM_SETFONT, (WPARAM)getStdFont(5), 0);
    return label;
}

static inline void mlxCreateEdit(EditText& edit, HWND parent, int id, int x, int y, int cx, int cy)
{
    edit.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", L"", 0x50010080, x, y, cx, cy, parent, (HMENU)(INT_PTR)id, 0, 0);
    if (edit.m_hWnd)
    {
        edit.setFontId(edit.m_fontId);
        SetWindowSubclass(edit.m_hWnd, tabSubclassProc, 0x65646974 , (DWORD_PTR)&edit);
    }
}

static inline void mlxCreateButton(Button& button, HWND parent, const wchar_t* text, int id, int x, int y)
{
    button.m_hWnd = CreateWindowExW(0, L"Button", text, 0x50000000, x, y, 100, 24, parent, (HMENU)(INT_PTR)id, 0, 0);
    if (button.m_hWnd)
        button.setFontId(button.m_fontId);
    else
        GetLastError();
}

static inline void mlxCreateAlphaCombo(ComboBox& combo, HWND parent, int id, int x)
{
    combo.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"ComboBox", 0, (combo.m_dropLines & 3) | 0x50010000,
                                   x, 358, 150, 120, parent, (HMENU)(INT_PTR)id, 0, 0);
    if (combo.m_hWnd)
        combo.setFontId(combo.m_fontId);
    else
        GetLastError();
    if (SendMessageW(combo.m_hWnd, CB_ADDSTRING, 0, (LPARAM)L"Visibility") != -1)
    {
        int count = (int)SendMessageW(combo.m_hWnd, CB_GETCOUNT, 0, 0);
        if (count >= 0)
            combo.m_items.put(count, L"Visibility");
    }
    if (SendMessageW(combo.m_hWnd, CB_ADDSTRING, 0, (LPARAM)L"Glow") != -1)
    {
        int count = (int)SendMessageW(combo.m_hWnd, CB_GETCOUNT, 0, 0);
        if (count >= 0)
            combo.m_items.put(count, L"Glow");
    }
}

static inline void mlxSelectOnly(HWND list, int index)
{
    LVITEMW item;
    item.stateMask = LVIS_SELECTED;
    item.state = 0;
    SendMessageW(list, LVM_SETITEMSTATE, (WPARAM)-1, (LPARAM)&item);
    item.stateMask = LVIS_SELECTED;
    item.state = LVIS_SELECTED;
    SendMessageW(list, LVM_SETITEMSTATE, (WPARAM)index, (LPARAM)&item);
}

bool MlxView::onCreate()
{
    if (!ModelView::onCreate())
        return false;

    const wchar_t* fileName = DOC_FILENAME(DOCVIEW_DOC(this));
    if (!fileName)
        return false;
    if (wcslen(fileName) != 8 || fileName[4] != L'.')
        return false;
    FIELD(DWORD, this, 0x44C) = ((const DWORD*)fileName)[0];
    FIELD(DWORD, this, 0x450) = ((const DWORD*)fileName)[1];
    MLXV_RACE(this) = mlxRaceIndex(MLXV_CODE(this));

    String title(L"Gears for ", (unsigned)-1);
    title << mlxRaceDescription(MLXV_CODE(this));
    MLXV_STATIC(this, 0) = createStatic(15, 10, 640, 18, title.m_buffer, 0, 5);
    MLXV_LIST(this).create(m_hWnd, (HMENU)0x65, 15, 30, 640, 237, 0x10810000, 0, 0);
    MLXV_STATIC(this, 1) = createStatic(15, 318, 100, 14, L"Name:", 0, 5);
    mlxCreateEdit(MLXV_NAME(this), m_hWnd, 0x66, 15, 334, 90, 18);

    MLXV_STATIC(this, 2) = createStatic(120, 312, 25, 18, L"DDS", 2, 5);
    for (int i = 0, x = 153; x < 753; ++i, x += 150)
        mlxCreateEdit(MLXV_DDS(this, i), m_hWnd, i + 110, x, 310, 151, 18);

    MLXV_STATIC(this, 3) = mlxCreateLabel(m_hWnd, L"3DC", 0x50000002, 120, 336, 25, 18);
    for (int i = 0, x = 153; x < 753; ++i, x += 150)
        mlxCreateEdit(MLXV_3DC(this, i), m_hWnd, i + 120, x, 334, 151, 18);

    MLXV_STATIC(this, 4) = mlxCreateLabel(m_hWnd, L"Alpha", 0x50000002, 105, 360, 40, 18);
    for (int i = 0, x = 153; x < 753; ++i, x += 150)
        mlxCreateAlphaCombo(MLXV_ALPHA(this, i), m_hWnd, i + 130, x);

    mlxCreateButton(MLXV_BTN_RENAME(this), m_hWnd, L"Rename Files", 0xC8, 313, 388);
    mlxCreateButton(MLXV_BTN_APPEND(this), m_hWnd, L"Append Model", 0xC9, 423, 388);
    mlxCreateButton(MLXV_BTN_UPDATE(this), m_hWnd, L"Update Model", 0xCA, 533, 388);
    MLXV_STATIC(this, 5) = mlxCreateLabel(m_hWnd, L"The model is used by:", 0x50000000, 15, 428, 135, 18);

    MLXV_GEARS(this, 0).create(m_hWnd, (HMENU)0x8C, 153, 420, 301, 81, 0x10800000, 0, 0);
    MLXV_GEARS(this, 2).create(m_hWnd, (HMENU)0x8D, 453, 420, 301, 81, 0x10800000, 0, 0);
    MLXV_GEARS(this, 1).create(m_hWnd, (HMENU)0x8E, 153, 500, 301, 81, 0x10800000, 0, 0);
    MLXV_GEARS(this, 3).create(m_hWnd, (HMENU)0x8F, 453, 500, 301, 81, 0x10800000, 0, 0);

    if (MLXV_MODELS(this)->isEmpty())
    {
        String prefix = getDirectory();
        prefix << MLXV_CODE(this);
        prefix << L'_';
        if (loadMltFiles(prefix))
            return false;
        setState(1);
    }
    MLXV_LIST(this).setItemCount(MLXV_MODELS(this)->size());
    PostMessageW(m_hWnd, WM_EXITSIZEMOVE, 0, 0);
    return true;
}

void MlxView::onSize(WPARAM type, int cx, int cy)
{
    HDWP dwp = BeginDeferWindowPos(27);
    if (!dwp)
        return;
    int barHeight = DOCVIEW_BAR_H(this);
    int height = cy - barHeight;
    dwp = DeferWindowPos(dwp, DOCVIEW_BAR(this), 0, 0, cy - barHeight, cx, barHeight, SWP_NOZORDER | SWP_NOOWNERZORDER);
    dwp = DeferWindowPos(dwp, MLXV_LIST(this).m_hWnd, 0, 0, 0, 760, height - 322,
                         SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOOWNERZORDER);
    const UINT moveOnly = SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER;
    int y = height - 282;
    dwp = DeferWindowPos(dwp, MLXV_STATIC(this, 1), 0, 15, y + 10, 0, 0, moveOnly);
    int y2 = y + 24;
    dwp = DeferWindowPos(dwp, MLXV_NAME(this).m_hWnd, 0, 15, y2, 0, 0, moveOnly);
    dwp = DeferWindowPos(dwp, MLXV_STATIC(this, 2), 0, 120, y + 4, 0, 0, moveOnly);
    for (int i = 0, x = 151; x < 751; ++i, x += 150)
        dwp = DeferWindowPos(dwp, MLXV_DDS(this, i).m_hWnd, 0, x, y, 0, 0, moveOnly);
    dwp = DeferWindowPos(dwp, MLXV_STATIC(this, 3), 0, 120, y2 + 4, 0, 0, moveOnly);
    for (int i = 0, x = 151; x < 751; ++i, x += 150)
        dwp = DeferWindowPos(dwp, MLXV_3DC(this, i).m_hWnd, 0, x, y2, 0, 0, moveOnly);
    dwp = DeferWindowPos(dwp, MLXV_STATIC(this, 4), 0, 105, y2 + 30, 0, 0, moveOnly);
    for (int i = 0, x = 151; x < 751; ++i, x += 150)
        dwp = DeferWindowPos(dwp, MLXV_ALPHA(this, i).m_hWnd, 0, x, y2 + 24, 0, 0, moveOnly);
    dwp = DeferWindowPos(dwp, MLXV_BTN_RENAME(this).m_hWnd, 0, 433, y2 + 54, 0, 0, moveOnly);
    dwp = DeferWindowPos(dwp, MLXV_BTN_APPEND(this).m_hWnd, 0, 543, y2 + 54, 0, 0, moveOnly);
    dwp = DeferWindowPos(dwp, MLXV_BTN_UPDATE(this).m_hWnd, 0, 653, y2 + 54, 0, 0, moveOnly);
    dwp = DeferWindowPos(dwp, MLXV_STATIC(this, 5), 0, 15, y2 + 95, 0, 0, moveOnly);
    dwp = DeferWindowPos(dwp, MLXV_GEARS(this, 0).m_hWnd, 0, 151, y2 + 86, 0, 0, moveOnly);
    dwp = DeferWindowPos(dwp, MLXV_GEARS(this, 2).m_hWnd, 0, 451, y2 + 86, 0, 0, moveOnly);
    dwp = DeferWindowPos(dwp, MLXV_GEARS(this, 1).m_hWnd, 0, 151, y2 + 166, 0, 0, moveOnly);
    dwp = DeferWindowPos(dwp, MLXV_GEARS(this, 3).m_hWnd, 0, 451, y2 + 166, 0, 0, moveOnly);
    EndDeferWindowPos(dwp);
}

void MlxView::placeView3D()
{
    RECT rc;
    getWindowRect(&rc);
    PopupView3D& view3D = MV_VIEW3D(this);
    view3D.move(rc.right, rc.top);
    view3D.resize(400, 680);
    view3D.show(true);
}

HRESULT MlxView::store()
{
    oStream out(false, 1200);
    MLXV_MODELS(this)->write(out, 'file');
    BYTE* doc = DOCVIEW_DOC(this);
    FileTree* entry = FRAME_FOLDERS().findFile(DOC_PATH(doc).m_buffer);
    if (entry)
    {
        if (g_contentProvider)
            g_contentProvider->writeFile(entry, &out, false);
    }
    else
    {
        out.save(DOC_PATH(doc).m_buffer);
    }
    String prefix = getDirectory();
    prefix << MLXV_CODE(this);
    prefix << L'_';
    int result = saveMltFiles(prefix);
    return result;
}

int MlxView::loadMltFiles(const String& prefix)
{
    ModelsMLT models;
    for (int i = 0; i < 4; ++i)
    {
        models.deleteAll();
        Bytes file;
        String path = prefix + g_mlxPartNames[i] + L".mlt";
        if (!g_studioFrame->loadFromFile(&file, &path, false))
        {
            models.read(file, 'file');
            int count = models.m_count;
            for (int k = 0; k < count; ++k)
            {
                ModelMLT* mlt = models.m_items[k];
                ModelsMLX* list = MLXV_MODELS(this);
                ModelMLX* mlx = 0;
                int j = 0;
                for (; j < list->m_count; ++j)
                {
                    if (MLX_ID(list->m_items[j]) == MLT_ID(mlt))
                        break;
                }
                if (j < list->m_count)
                    mlx = list->m_items[j];
                if (!mlx)
                {
                    mlx = new ModelMLX();
                    MLX_ID(mlx) = MLT_ID(mlt);
                    MLXV_MODELS(this)->add(mlx);
                }
                MLX_DDS(mlx, i) = MLT_DDS(mlt);
                MLX_3DC(mlx, i) = MLT_3DC(mlt);
                MLX_ALPHA(mlx, i) = MLT_ALPHA(mlt);
            }
        }
    }
    return 0;
}

int MlxView::saveMltFiles(const String& prefix)
{
    ModelsMLT models[4];
    int count = MLXV_MODELS(this)->size();
    for (int k = 0; k < count; ++k)
    {
        ModelMLX* mlx = MLXV_MODELS(this)->m_items[k];
        for (int i = 0; i < 4; ++i)
        {
            ModelMLT* mlt = new ModelMLT(k, MLX_DDS(mlx, i).m_buffer, MLX_3DC(mlx, i).m_buffer, MLX_ALPHA(mlx, i));
            if (mlt)
                models[i].add(mlt);
        }
    }

    Bytes file;
    for (int i = 0; i < 4; ++i)
    {
        file.wipe();
        models[i].write(file, 'file');
        String path = prefix + g_mlxPartNames[i] + L".mlt";
        FileTree* entry = FRAME_FOLDERS().findFile(path.m_buffer);
        if (entry)
        {
            if (g_contentProvider)
                g_contentProvider->writeFile(entry, static_cast<oStream*>(&file), false);
        }
        else
        {
            file.save(path.m_buffer);
        }
    }
    return 0;
}

LRESULT MlxView::defWindowProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg != WM_GETMINMAXINFO)
        return ModelView::defWindowProc(msg, wParam, lParam);
    MINMAXINFO* mmi = (MINMAXINFO*)lParam;
    mmi->ptMinTrackSize.x = 800;
    mmi->ptMaxTrackSize.x = 800;
    mmi->ptMaxSize.x = 800;
    mmi->ptMinTrackSize.y = 680;
    return 0;
}

bool MlxView::onCommand(UINT code, HWND ctrl, UINT id)
{
    if (!code)
    {
        int index;
        switch (id)
        {
        case 201:
        {
            ModelMLX* mlx = new ModelMLX();
            MLX_ID(mlx) = MLXV_MODELS(this)->size();
            {
                String text;
                MLXV_NAME(this).getText(&text);
                MLX_NAME(mlx) = text;
            }
            for (int i = 0; i < 4; ++i)
            {
                {
                    String text;
                    MLXV_DDS(this, i).getText(&text);
                    MLX_DDS(mlx, i) = text;
                }
                {
                    String text;
                    MLXV_3DC(this, i).getText(&text);
                    MLX_3DC(mlx, i) = text;
                }
            }
            MLXV_MODELS(this)->add(mlx);
            MLXV_LIST(this).setItemCount(MLXV_MODELS(this)->size());
            index = MLX_ID(mlx);
            mlxSelectOnly(MLXV_LIST(this).m_hWnd, index);
            break;
        }
        case 202:
        {
            index = MLXV_LIST(this).getNextSelected(0);
            if (index < 0)
                return true;
            ModelMLX* mlx = MLXV_MODELS(this)->get(index);
            if (mlx)
            {
                for (int i = 0; i < 4; ++i)
                {
                    {
                        String text;
                        MLXV_DDS(this, i).getText(&text);
                        MLX_DDS(mlx, i) = text;
                    }
                    {
                        String text;
                        MLXV_3DC(this, i).getText(&text);
                        MLX_3DC(mlx, i) = text;
                    }
                    MLX_ALPHA(mlx, i) = (int)SendMessageW(MLXV_ALPHA(this, i).m_hWnd, CB_GETCURSEL, 0, 0);
                }
            }
            {
                String text;
                MLXV_NAME(this).getText(&text);
                MLX_NAME(mlx) = text;
            }
            mlxSelectOnly(MLXV_LIST(this).m_hWnd, index);
            break;
        }
        case 200:
        {
            index = MLXV_LIST(this).getNextSelected(0);
            if (index < 0)
                return true;
            ModelMLX* mlx = MLXV_MODELS(this)->get(index);
            if (mlx)
            {
                String folder = getDirectory();
                String ddsDir = folder + L"DDS\\";
                String tdcDir = folder + L"3DC\\";
                for (int i = 0; i < 4; ++i)
                {
                    String newDds;
                    MLXV_DDS(this, i).getText(&newDds);
                    if (MLX_DDS(mlx, i).differsNoCase(newDds))
                    {
                        BOOL moved;
                        {
                            String to = ddsDir + newDds;
                            String from = ddsDir + MLX_DDS(mlx, i);
                            moved = MoveFileW(from.m_buffer, to.m_buffer);
                        }
                        if (moved)
                            MLX_DDS(mlx, i) = newDds;
                    }
                    String newTdc;
                    MLXV_3DC(this, i).getText(&newTdc);
                    if (MLX_3DC(mlx, i).differsNoCase(newTdc))
                    {
                        BOOL moved;
                        {
                            String to = tdcDir + newTdc;
                            String from = tdcDir + MLX_3DC(mlx, i);
                            moved = MoveFileW(from.m_buffer, to.m_buffer);
                        }
                        if (moved)
                            MLX_3DC(mlx, i) = newTdc;
                    }
                }
            }
            mlxSelectOnly(MLXV_LIST(this).m_hWnd, index);
            break;
        }
        default:
            return ModelView::onCommand(code, ctrl, id);
        }
        MLXV_LIST(this).updateItem(index);
        selectModel(index);
        setModified(true);
        return true;
    }
    return ModelView::onCommand(code, ctrl, id);
}

bool MlxView::onNotify(NMHDR* hdr, LRESULT* result)
{
    int notifyCode = (int)hdr->code;
    if (notifyCode == -5)
        return true;
    UINT_PTR from = hdr->idFrom;
    if (from >= 0x8C && from <= 0x8F && notifyCode == -177)
    {
        HWND target = 0;
        switch (from)
        {
        case 0x8C:
            SendMessageW(MLXV_GEARS(this, 0).m_hWnd, WM_NOTIFY, 0, (LPARAM)hdr);
            return true;
        case 0x8D:
            SendMessageW(MLXV_GEARS(this, 2).m_hWnd, WM_NOTIFY, 0, (LPARAM)hdr);
            return true;
        case 0x8E:
            SendMessageW(MLXV_GEARS(this, 1).m_hWnd, WM_NOTIFY, 0, (LPARAM)hdr);
            return true;
        case 0x8F:
            target = MLXV_GEARS(this, 3).m_hWnd;
        default:
            SendMessageW(target, WM_NOTIFY, 0, (LPARAM)hdr);
            return true;
        }
    }
    if (notifyCode == -121)
    {
        *result = 1;
        return true;
    }
    Control* child = MV_CHILD(this);
    if (child && child->m_id == (int)from)
        return child->onNotify(hdr, result);
    return false;
}

void MlxView::selectModel(int index)
{
    MV_SELECTED(this) = index;
    MV_VIEW3D(this).clearModel();
    ModelMLX* mlx = MLXV_MODELS(this)->get(index);
    if (mlx)
    {
        if (MV_VIEW3D(this).isVisible())
        {
            Strings tdcNames(4);
            tdcNames.m_allowDuplicates = true;
            Strings ddsNames(4);
            ddsNames.m_allowDuplicates = true;
            for (int i = 0; i < 4; ++i)
            {
                ddsNames.add(MLX_DDS(mlx, i).m_buffer);
                tdcNames.add(MLX_3DC(mlx, i).m_buffer);
            }
            MetaBody* body = new MetaBody();
            body->loadModels(getDirectory(), &tdcNames, &ddsNames);
            MV_VIEW3D(this).setModel(body);
        }

        MLXV_NAME(this).setText(MLX_NAME(mlx).m_buffer);
        for (int i = 0; i < 4; ++i)
        {
            MLXV_3DC(this, i).setText(MLX_3DC(mlx, i).m_buffer);
            MLXV_DDS(this, i).setText(MLX_DDS(mlx, i).m_buffer);
            SendMessageW(MLXV_ALPHA(this, i).m_hWnd, CB_SETCURSEL, (WPARAM)MLX_ALPHA(mlx, i), 0);
        }

        wchar_t race = MLXV_CODE(this)[0];
        bool light = race == L'h' || race == L'e';
        primitiveVector<unsigned char> upperTypes;
        primitiveVector<unsigned char> lowerTypes;
        primitiveVector<unsigned char> glovesTypes;
        primitiveVector<unsigned char> bootsTypes;
        if (light)
        {
            upperTypes.add(17);  upperTypes.add(67);  upperTypes.add(73);
            lowerTypes.add(18);  lowerTypes.add(68);  lowerTypes.add(74);
            glovesTypes.add(20); glovesTypes.add(70); glovesTypes.add(76);
            bootsTypes.add(21);  bootsTypes.add(71);  bootsTypes.add(77);
        }
        else
        {
            upperTypes.add(32);  upperTypes.add(82);  upperTypes.add(88);
            lowerTypes.add(33);  lowerTypes.add(83);  lowerTypes.add(89);
            glovesTypes.add(35); glovesTypes.add(85); glovesTypes.add(91);
            bootsTypes.add(36);  bootsTypes.add(86);  bootsTypes.add(92);
        }
        MLXV_GEARS(this, 0).fill(upperTypes, MV_SELECTED(this), MLXV_RACE(this));
        MLXV_GEARS(this, 1).fill(lowerTypes, MV_SELECTED(this), MLXV_RACE(this));
        MLXV_GEARS(this, 3).fill(glovesTypes, MV_SELECTED(this), MLXV_RACE(this));
        MLXV_GEARS(this, 2).fill(bootsTypes, MV_SELECTED(this), MLXV_RACE(this));
    }
    else
    {
        primitiveVector<unsigned char> none;
        MLXV_GEARS(this, 0).fill(none, 0, 0);
        MLXV_GEARS(this, 1).fill(none, 0, 0);
        MLXV_GEARS(this, 2).fill(none, 0, 0);
        MLXV_GEARS(this, 3).fill(none, 0, 0);
    }
}

void MlxView::editModel(int index)
{
    POINT pt;
    pt.x = 0;
    pt.y = 0;
    GetCursorPos(&pt);
    LONG range;
    int column = MLXV_LIST(this).columnFromPoint(&pt, &range, 0);
    if (column < 0)
        return;
    int part = column - 2;
    if (!MLXV_MODELS(this)->get(index))
        return;

    String tdcName;
    MLXV_3DC(this, part).getText(&tdcName);
    String path = getDirectory();
    path << L"3DC\\";
    path.append(tdcName.m_buffer, tdcName.m_length);
    if (g_studioFrame->openDocument(0, path.m_buffer, 0))
    {
        ThreeDxView* view = dynamic_cast<ThreeDxView*>(g_studioFrame->findDocument(path.m_buffer));
        String ddsName;
        MLXV_DDS(this, part).getText(&ddsName);
        const wchar_t* texture = ddsName.m_buffer;
        ThreeDxCtrlsPanel* panel = dynamic_cast<ThreeDxCtrlsPanel*>(FIELD(CtrlsPanel*, view, 0x228));
        if (panel)
            PANEL_TEXTURE(panel).setText(texture);
    }
}
