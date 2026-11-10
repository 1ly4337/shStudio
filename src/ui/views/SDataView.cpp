#include "SDataView.h"
#include "Serializable.h"
#include "Bytes.h"
#include "Application.h"
#include "StudioFrame.h"
#include "AlertDial.h"
#include "SqlServer.h"
#include "RecordSet.h"
#include "ImportSQLEditor.h"
#include "ida_compat.h"
#include "_imports_E.h"
#include <shlwapi.h>
#include <cstdio>
#include <cwchar>

COMDLG_FILTERSPEC g_sdataFileTypes[1] = { { L"SData File", L"*.sdata" } };

SDataView::SDataView(void* file, Serializable* data)
    : DocView((FileTree*)file, data)
{
    if (Window_isClassicAppearance())
        m_bkColor = GetSysColor(COLOR_BTNFACE);
    else
        m_bkColor = 0xF0F0F0;
    FIELD(BYTE, this, 0xA5) = 1;
}

void SDataView::onRegisterClass(WNDCLASSEXW* wcx)
{
    wcx->hIcon = (HICON)LoadImageW(hModule, MAKEINTRESOURCEW(0x84), IMAGE_ICON,
                                   GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), LR_SHARED);
    wcx->hIconSm = (HICON)LoadImageW(hModule, MAKEINTRESOURCEW(0x84), IMAGE_ICON,
                                     GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_SHARED);
}

static void checkMenuItem(HMENU menu, UINT id, bool checked)
{
    MENUITEMINFOW mii;
    memset(&mii.fMask, 0, 0x2C);
    mii.cbSize = sizeof(MENUITEMINFOW);
    mii.fMask = MIIM_STATE;
    GetMenuItemInfoW(menu, id, FALSE, &mii);
    if ((mii.fState & 3) == 0)
    {
        UINT state = 0;
        if (checked)
            state = MFS_CHECKED;
        mii.fState = state;
        SetMenuItemInfoW(menu, id, FALSE, &mii);
    }
}

bool SDataView::onInitMenu(HMENU menu)
{
    setStatusText(0, 0);
    int mode = getViewMode() & 0xFFF;
    checkMenuItem(menu, 0xE401, mode == 0x400);
    checkMenuItem(menu, 0xE402, mode == 0x500);
    checkMenuItem(menu, 0xE403, mode == 0x600);
    checkMenuItem(menu, 0xE404, mode == 0x620);

    SqlServer* sql = (SqlServer*)g_studioFrame->getSqlConnection();
    bool canImport = sql && sql->m_connected && getSQLTableName();
    MENUITEMINFOW mii;
    memset(&mii.fMask, 0, 0x2C);
    UINT state = MFS_DISABLED;
    if (canImport)
        state = 0;
    mii.fState = state;
    mii.cbSize = sizeof(MENUITEMINFOW);
    mii.fMask = MIIM_STATE;
    SetMenuItemInfoW(menu, 0xE431, FALSE, &mii);
    return true;
}

bool SDataView::onCommand(UINT code, HWND ctrl, UINT id)
{
    if ((code | 1) != 1 || ctrl)
        return DocView::onCommand(code, ctrl, id);
    switch (id)
    {
    case 0xE401:
        setViewMode(0x400);
        return true;
    case 0xE402:
        setViewMode(0x500);
        return true;
    case 0xE403:
        setViewMode(0x600);
        return true;
    case 0xE404:
        setViewMode(0x620);
        return true;
    case 0xE430:
        mergeLanguageFile();
        return true;
    case 0xE431:
        importSQLDefinition();
        return true;
    case 0xE105:
        exportFile();
        return true;
    default:
        return DocView::onCommand(code, 0, id);
    }
}

void SDataView::fillScriptMenu(HMENU menu)
{
    appendMenuItem(menu, 0xE435);
    appendMenuItem(menu, 0xE430);
    appendMenuItem(menu, 0xE431);
}

COMDLG_FILTERSPEC* SDataView::getFilters(int* count)
{
    *count = 1;
    return g_sdataFileTypes;
}

bool SDataView::getScriptPath(String* path, const wchar_t* name)
{
    {
        String server;
        String* dir = g_studioFrame->getServerPath(&server);
        if (dir != path)
            *path = *dir;
    }
    if (!path->m_length)
        return false;
    BOOL hasScripts = PathFileExistsW((*path + L"scripts\\").m_buffer);
    if (hasScripts)
        *path << L"scripts\\";
    *path << name;
    path->removeExtension();
    *path << L".sql";
    return true;
}

void SDataView::exportFile()
{
    String path;
    FileTree* file = Doc::m_data;
    if (file)
    {
        path = FIELD(String, file, 0x30);
        path.removeExtension();
    }
    else
    {
        path = L"noname";
    }

    int filterIndex = 1;
    const wchar_t* filters[4] = {
        L"Comma-Separated Values File", L"*.csv",
        L"Structured Query Language File", L"*.sql",
    };
    if (FileDialogs_save(m_hWnd, &path, L"*.csv", (const COMDLG_FILTERSPEC*)filters, 2, (DWORD*)&filterIndex, 0))
        return;

    String ext;
    ext.append(PathFindExtensionW(path.m_buffer), (unsigned)-1);
    ext.toLowerInPlace();
    DWORD format;
    if (ext == L".csv")
        format = 'csv ';
    else if (ext == L".sql")
        format = 'sql ';
    else
        return;

    String text;
    if (!exportData(&text, format))
        return;

    FILE* stream = 0;
    _wfopen_s(&stream, path.m_buffer, L"wb");
    if (!stream)
        return;
    size_t written = 0;
    if (text.m_length)
    {
        int bom = 0xFEFF;
        fwrite(&bom, 2, 1, stream);
        written = fwrite(text.m_buffer, 2 * text.m_length, 1, stream);
    }
    fflush(stream);
    fclose(stream);
    if (written == 1 || (text.m_length != 0 ? ERROR_WRITE_FAULT : ERROR_NO_DATA) == 0)
        setStatusText(L"Data successfully exported.", 0);
}

void SDataView::mergeLanguageFile()
{
    Serializable* data = m_content;
    if (!data)
        return;

    String path;
    FileTree* file = Doc::m_data;
    if (file || (file = m_ownedData) != 0)
    {
        path = FIELD(String, file, 0x10);
        path.removeFileName();
    }
    else
    {
        String workspace;
        path = *g_studioFrame->getWorkspacePath(&workspace);
    }
    path.clear();

    String filter = getMergeFilter() + L"SData File\t*.sdata\t";
    unsigned length = filter.m_length;
    for (unsigned i = 0; i < length; ++i)
    {
        if (filter.m_buffer[i] == L'\t')
        {
            filter.m_buffer[i] = 0;
            length = filter.m_length;
        }
    }
    if (!FileDialogs_open(m_hWnd, &path, filter.m_buffer, L"Select a language file to merge", 1))
        return;

    UpdateWindow(m_hWnd);
    Bytes bytes;
    const wchar_t* fileName = path.m_buffer;
    try
    {
        if (bytes.aStream::load(fileName))
            throw 0;
    }
    catch (...)
    {
        AlertDial::alert(m_hWnd, L"The selected file cannot be loaded.", 0, MB_ICONERROR);
        return;
    }

    try
    {
        setStatusText(L"Updating text fields...", 0);
        data->load(bytes, (int)static_cast<StreamCallback*>(this), 0);
        data->setFileName(fileName);
    }
    catch (const wchar_t* fmt)
    {
        size_t count = wcslen(fmt) + wcslen(fileName) + 64;
        wchar_t* message = new wchar_t[count];
        swprintf_s(message, count, fmt, fileName);
        AlertDial::alert(m_hWnd, message, L"Error", MB_ICONERROR);
        delete[] message;
    }
    catch (...)
    {
        AlertDial::alert(m_hWnd, L"The selected file is not a valid names file.", 0, MB_ICONERROR);
    }
    onDataUpdated();
}

void SDataView::importSQLDefinition()
{
    const wchar_t* table = getSQLTableName();
    if (!m_content || !table)
        return;
    SqlServer* sql = (SqlServer*)g_studioFrame->getSqlConnection();
    if (!sql || !sql->m_connected)
    {
        setStatusText(L"A connection to the SQL server must be alive to import database definition.", 0);
        return;
    }

    String name(table, (unsigned)-1);
    int space = name.indexOf(L' ');
    if (space > 0)
        name = name.left(space);

    ImportSQLEditor dialog(name.m_buffer);
    if (dialog.createDialog(m_hWnd, 460, 150, MB_YESNO, L"Import SQL definition") && dialog.doModal() == IDYES)
    {
        bool captions = dialog.m_importCaptions;
        UpdateWindow(m_hWnd);
        try
        {
            String countSql(L"Select Count(*) from ", (unsigned)-1);
            RecordSet countRs = sql->query((countSql + table).m_buffer);
            if (!(countRs.m_rs && countRs.m_rs->getIterator()->next()))
                throw 1;
            int count;
            if (countRs.m_rs)
                count = countRs.m_rs->getIterator()->getIntAt(0);
            else
                count = 0;
            setStatusText(L"Updating definition...", 0);
            setProgress(0, count);

            String selectSql(L"Select * from ", (unsigned)-1);
            RecordSet rs = sql->query((selectSql + table).m_buffer);
            for (int i = 0; i < count; ++i)
            {
                setProgress(i, count);
                if (!rs.m_rs || !rs.m_rs->getIterator()->next())
                    throw 2;
                importSQLRow(&rs, captions);
            }
        }
        catch (...)
        {
        }
        onDataUpdated();
    }
}

void SDataView::onDataUpdated()
{
    setStatusText(0, 0);
    setModified(true);
}
