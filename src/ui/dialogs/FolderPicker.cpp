#include <windows.h>
#include <cwchar>
#include <cstring>
#include "FolderPicker.h"
#include "GDC.h"
#include "PaintDC.h"
#include "Tokeniser.h"
#include "WinAppearance.h"
#include "_imports_p09.h"
#include "_imports_g6.h"

static const wchar_t g_folderEllipsis[] = L"\x2026\\";

static const wchar_t* nextToken(Tokeniser& tok)
{
    if (!tok.m_text || tok.m_current >= tok.m_length)
        return 0;
    if (tok.m_current < 0)
    {
        tok.m_current = 0;
        return tok.m_text;
    }
    while (tok.m_current < tok.m_length && tok.m_text[tok.m_current])
        ++tok.m_current;
    if (tok.m_current == tok.m_length)
        return 0;
    ++tok.m_current;
    return &tok.m_text[tok.m_current];
}

static LONG textWidth(HDC hdc, const wchar_t* text)
{
    RECT measure;
    measure.left = 0;
    measure.top = 0;
    measure.right = 1;
    measure.bottom = 24;
    DrawTextW(hdc, text, -1, &measure, DT_CALCRECT);
    return measure.right;
}

FolderPicker::FolderPicker()
{
    m_state = 0;
    m_browse = false;
    m_tracking = false;
    m_button = (HBITMAP)LoadImageW(hModule, MAKEINTRESOURCEW(0x15E), IMAGE_BITMAP, 0, 0, LR_LOADTRANSPARENT);
}

FolderPicker::~FolderPicker()
{
    if (m_button)
        DeleteObject(m_button);
}

LRESULT FolderPicker::defWindowProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_SETFOCUS)
    {
        m_state |= 0x10;
        invalidate(false);
        return 0;
    }
    if (msg == WM_KILLFOCUS)
    {
        m_state &= ~0x10;
        invalidate(false);
        return 0;
    }
    if (msg != WM_PAINT)
        return DefWindowProcW(m_hWnd, msg, wParam, lParam);
    PaintDC dc(m_hWnd);
    paint(&dc);
    return 0;
}

void FolderPicker::paint(GDC* dc)
{
    HDC hdc = dc->m_hdc;
    RECT rc;
    getClientRect(&rc);
    Window_isClassicAppearance();
    FillRect(hdc, &rc, getSolidBrush(GetSysColor(COLOR_WINDOW)));
    bool pressed = ((m_state >> 2) & 1) != 0;
    if (m_button)
    {
        HDC memory = CreateCompatibleDC(hdc);
        HGDIOBJ previous = SelectObject(memory, m_button);
        int cell = 0;
        if (pressed)
            cell = 16;
        BitBlt(hdc, rc.right - 18, 2, 16, 16, memory, cell, 0, SRCCOPY);
        SelectObject(memory, previous);
        DeleteDC(memory);
        if ((m_state & 0x400) != 0)
        {
            Window_isClassicAppearance();
            dc->frameRect(rc.right - 20, 0, rc.right - 1, 19, GetSysColor(COLOR_BTNSHADOW));
        }
    }
    rc.right -= 24;
    Window_isClassicAppearance();
    dc->frameRect(&rc, GetSysColor(COLOR_BTNSHADOW));
    LONG top, left, right;
    if ((m_state & 0x44) == 0x40)
    {
        ++rc.top;
        --rc.bottom;
        ++rc.left;
        --rc.right;
        dc->frameRect(&rc, 0xFFB9B9);
        ++rc.bottom;
        top = rc.top - 1;
        left = rc.left - 1;
        right = rc.right + 1;
    }
    else
    {
        right = rc.right;
        top = rc.top;
        left = rc.left;
    }
    rc.left = left + 4;
    rc.top = top + 3;
    rc.right = right - 4;

    HGDIOBJ previousFont = SelectObject(hdc, getStdFont(0));
    if (!dc->m_saved0C)
        dc->m_saved0C = previousFont;

    LONG width = m_path.m_buffer ? textWidth(hdc, m_path.m_buffer) : 0;
    if (width <= rc.right - rc.left)
    {
        if (m_path.m_buffer)
            DrawTextW(hdc, m_path.m_buffer, -1, &rc, DT_PATH_ELLIPSIS | DT_VCENTER);
        return;
    }

    String text(m_path);
    Tokeniser tok('\\', false);
    tok.set(text.m_buffer);
    if (tok.m_count >= 2)
    {
        for (;;)
        {
            nextToken(tok);
            int widest = -1;
            LONG widestWidth = 0;
            int last = tok.m_count - 1;
            if (last <= 1)
                break;
            for (int i = 1; i < last; ++i)
            {
                const wchar_t* token = nextToken(tok);
                LONG tokenWidth = token ? textWidth(hdc, token) : 0;
                if (widestWidth < tokenWidth)
                {
                    widest = i;
                    widestWidth = tokenWidth;
                }
            }
            if (widest == -1)
                break;

            text.clear();
            tok.m_current = -1;
            for (int i = 0; i < last; ++i)
            {
                if (i != widest)
                {
                    const wchar_t* token = nextToken(tok);
                    text << token;
                    text << L'\\';
                }
                else
                {
                    nextToken(tok);
                    if (text.m_length >= 2 && !_wcsnicmp(&text.m_buffer[text.m_length - 2], g_folderEllipsis, 2))
                        continue;
                    text << g_folderEllipsis;
                }
            }
            const wchar_t* token = nextToken(tok);
            if (token && *token)
            {
                text << token;
                text << L'\\';
            }

            LONG newWidth = text.m_buffer ? textWidth(hdc, text.m_buffer) : 0;
            if (newWidth < rc.right - rc.left)
                break;
            tok.set(text.m_buffer);
            if (tok.m_count < 2)
                break;
        }
    }
    if (text.m_buffer)
        DrawTextW(hdc, text.m_buffer, -1, &rc, DT_PATH_ELLIPSIS | DT_VCENTER);
}

bool FolderPicker::onLButton(WPARAM keys, POINT* pt, bool down)
{
    if (down)
    {
        if (m_browse)
        {
            String path(m_path);
            if (FileDialogs_selectFolder(GetParent(m_hWnd), &path, m_title.m_buffer, 0, 0, 0))
            {
                m_path = path;
                invalidate(false);
            }
            return true;
        }
        PostMessageW(GetParent(m_hWnd), WM_COMMAND, LOWORD(m_childId), (LPARAM)m_hWnd);
    }
    m_state |= 0x40;
    invalidate(false);
    return true;
}

bool FolderPicker::onMouseMove(WPARAM keys, POINT* pt)
{
    RECT rc;
    getClientRect(&rc);
    if (!PtInRect(&rc, *pt))
    {
        onMouseLeave();
        return true;
    }
    DWORD hot = 0;
    if (pt->x >= rc.right - 24)
    {
        if (pt->x > rc.right - 20)
            hot = 0x400;
    }
    else
    {
        hot = 0x40;
    }
    if (m_state != hot)
    {
        m_state = hot;
        invalidate(false);
        TRACKMOUSEEVENT track;
        track.hwndTrack = m_hWnd;
        track.cbSize = sizeof(TRACKMOUSEEVENT);
        track.dwFlags = TME_HOVER | TME_LEAVE;
        track.dwHoverTime = HOVER_DEFAULT;
        m_tracking = TrackMouseEvent(&track) != 0;
        return true;
    }
    return true;
}

bool FolderPicker::onMouseLeave()
{
    m_state &= 0xFFFFFBBF;
    m_tracking = false;
    invalidate(false);
    return true;
}
