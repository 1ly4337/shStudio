#include "TextView.h"
#include "FileTree.h"
#include "Bytes.h"
#include "aStream.h"
#include "ChooseCodepageDialog.h"
#include "gdi_cache.h"
#include "AlertDial.h"
#include "FileDialogs.h"
#include "ViewFilters.h"
#include "p10_vcall.h"
#include "Application.h"
#include "_imports_p10.h"
#include <commctrl.h>
#include <richedit.h>

static inline String& ftPath(FileTree* f) { return FIELD(String, f, 0x10); }
static inline String& ftName(FileTree* f) { return FIELD(String, f, 0x30); }
static inline int&    ftType(FileTree* f) { return FIELD(int, f, 0x140); }
#define APP_READ(a, b, s, dec) P10_VCALL((a), 0x14C, HRESULT, Bytes*, const String*, bool)((a), (b), (s), (dec))

bool TextView::onCreate()
{
    if (!Popup::onCreate())
        return false;
    FileTree* data = m_data;
    if (data && (P10_VCALL(data, 0x08, int)(data) & 2) != 0)
        setModified(true);
    m_modified = true;
    setModified(false);
    RECT rc;
    getClientRect(&rc);
    m_edit.m_hWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", L"", 0x50313044u, 0, 0, rc.right, rc.bottom,
                                    m_hWnd, (HMENU)101, 0, 0);
    if (!m_edit.m_hWnd)
        return false;
    m_edit.setFontId(m_edit.m_fontId);
    SetWindowSubclass(m_edit.m_hWnd, tabSubclassProc, 0x65646974u , (DWORD_PTR)&m_edit);
    if (m_readOnly)
        return loadText();
    m_edit.setText(m_text.m_buffer);
    return true;
}

bool TextView::loadText()
{
    bool result;
    Bytes bytes(0, 0u);
    if (APP_READ(g_studioFrame, &bytes, &ftPath(m_data), false))
        return false;
    aStream& stream = static_cast<aStream&>(static_cast<iStream&>(bytes));
    int size = stream.m_size;
    if (size < 2)
        return false;
    static_cast<oStream&>(bytes) << (char)0 << (char)0;
    int length = size + 2;
    const char* text = (const char*)(stream.m_pos + stream.m_data);
    if (*(const WORD*)text == 0xFEFF)
    {
        ftType(m_data) = 1200;
        m_84 = 1200;
        m_edit.setText((const WCHAR*)(text + 2));
        setState(1);
        SetFocus(m_edit.m_hWnd);
        return true;
    }
    if (*(const WORD*)text == 0xBBEF)
    {
        length -= 3;
        text += 3;
        if (length <= 0)
            return false;
        m_84 = 65001;
        ftType(m_data) = 65001;
    }
    if (!m_84)
    {
        m_84 = ftType(m_data);
        if (!m_84)
            m_84 = ChooseCodepageDialog::run(m_hWnd, ftName(m_data).m_buffer);
    }
    if (m_84 == -1)
        return false;
    if (!m_84)
        m_84 = g_layoutCodePage;
    String s;
    s.assign(text, length, (UINT)m_84);
    unsigned i = 0;
    bool hasCr = false;
    if (s.m_length)
    {
        while (s.m_buffer[i] != 13)
        {
            if (++i >= s.m_length)
                break;
        }
        hasCr = i < s.m_length;
    }
    if (!hasCr)
    {
        s = s.lfToCrLf();
        setModified(true);
        setStatusText(L"Unix EOL converted to Windows EOL", 0);
    }
    m_edit.setText(s.m_buffer);
    setState(1);
    SetFocus(m_edit.m_hWnd);
    result = true;
    return result;
}

COMDLG_FILTERSPEC g_textFileFilter[1] = { { L"Text File", L"*.txt" } };

#define FRAME_WRITE(o, b, s, enc) P10_VCALL((o), 0x150, HRESULT, Bytes*, const String*, bool)((o), (b), (s), (enc))

bool TextView::onCommand(UINT code, HWND ctrl, UINT id)
{
    if ((code | 1) != 1 || ctrl)
    {
        if (code == EN_CHANGE && ctrl == m_edit.m_hWnd)
        {
            setModified(true);
            return true;
        }
        return Doc::onCommand(code, ctrl, id);
    }
    CHARRANGE all;
    all.cpMin = 0;
    all.cpMax = -1;
    switch (id)
    {
    case 0xE103:
        if (saveText())
            setModified(false);
        return true;
    case 0xE120:
        SendMessageW(m_edit.m_hWnd, EM_UNDO, 0, 0);
        return true;
    case 0xE121:
        SendMessageW(m_edit.m_hWnd, WM_USER + 84 , 0, 0);
        return true;
    case 0xE122:
        SendMessageW(m_edit.m_hWnd, WM_CUT, 0, 0);
        return true;
    case 0xE123:
        SendMessageW(m_edit.m_hWnd, WM_COPY, 0, 0);
        return true;
    case 0xE124:
        SendMessageW(m_edit.m_hWnd, WM_PASTE, 0, 0);
        return true;
    case 0xE125:
        SendMessageW(m_edit.m_hWnd, WM_USER + 55 , 0, (LPARAM)&all);
        return true;
    default:
        return Doc::onCommand(code, 0, id);
    }
}

bool TextView::onClose()
{
    bool ok = true;
    if (m_modified)
    {
        String text(L"Do you want to save the changes made to:");
        FileTree* data = m_data;
        (text << L"\r\n").append(ftPath(data).m_buffer, ftPath(data).m_length);
        int answer = AlertDial::alert(m_hWnd, text.m_buffer, 0, MB_ICONQUESTION | MB_YESNOCANCEL);
        if (answer == IDCANCEL)
            ok = false;
        else if (answer == IDYES)
            ok = saveText();
    }
    if (ok)
        setState(0);
    return ok;
}

HRESULT TextView::load()
{
    FileTree* data = m_data;
    if (!data)
        return 4306;
    m_84 = ftType(data);
    loadText();
    return 0;
}

bool TextView::saveText()
{
    if (!m_readOnly)
    {
        MessageBeep(MB_ICONHAND);
        return false;
    }
    bool result;
    String text;
    m_edit.getText(&text);
    size_t size = 2 * text.m_length;
    Bytes bytes(0, 0u);
    aStream& stream = static_cast<aStream&>(static_cast<iStream&>(bytes));
    int codepage = m_84;
    if (codepage == 1200)
    {
        int need = (int)size + 2;
        if (stream.m_capacity < need)
            stream.grow(need - stream.m_capacity);
        WORD* dst = (WORD*)(stream.m_pos + stream.m_data + 2);
        dst[-1] = 0xFEFF;
        memcpy(dst, text.m_buffer, size);
        if (need <= stream.m_capacity)
            stream.m_size = need;
    }
    else
    {
        int count = (int)text.m_length;
        const char* defaultChar = "?";
        if (codepage == 65001)
            defaultChar = 0;
        const wchar_t* wide = text.m_buffer;
        if ((int)text.m_length > 0 && !text.m_buffer[text.m_length - 1])
            count = (int)text.m_length - 1;
        int bytesNeeded = WideCharToMultiByte(codepage, 0, wide, count, 0, 0, defaultChar, 0);
        if (bytesNeeded <= 0)
            return false;
        char* buffer = new char[bytesNeeded];
        int written = WideCharToMultiByte(m_84, 0, wide, count, buffer, bytesNeeded, defaultChar, 0);
        if (written > 0)
        {
            if (m_84 == 65001)
            {
                static const unsigned char bom[3] = { 0xEF, 0xBB, 0xBF };
                for (int i = 0; i < 3; ++i)
                {
                    if (stream.m_size + 65 <= stream.m_capacity || stream.grow(1))
                        stream.m_data[stream.m_size++] = bom[i];
                }
            }
            stream.append(buffer, written);
        }
        delete[] buffer;
        if (!written)
            return false;
    }
    FileTree* data = m_data;
    if (!ftPath(data).m_length)
    {
        DWORD typeIndex = 1;
        if (FileDialogs_save(m_hWnd, &ftPath(data), L".txt", g_textFileFilter, 1, &typeIndex, 0) || !ftPath(data).m_length)
            return false;
    }
    result = FRAME_WRITE(g_mainFrame, &bytes, &ftPath(m_data), false) == 0;
    return result;
}

COMDLG_FILTERSPEC* TextView::getFilters(int* count)
{
    *count = 1;
    return g_textFileFilter;
}
