#include <windows.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <commctrl.h>
#include <cstring>
#include "FileTree.h"
#include "FolderTree.h"
#include "iStream.hpp"
#include "oStream.hpp"
#include "Bytes.h"
#include "ContentProvider.h"
#include "DataObject.h"
#include "dragdrop.h"

#pragma warning(disable : 4066)

static wchar_t g_fileTimeText[256];

static int compareInt64(__int64 a, __int64 b)
{
    if (a == b)
        return 0;
    return a < b ? -1 : 1;
}

FileTree::FileTree()
    : m_path()
    , m_name()
    , m_time()
{
    init();
}

FileTree::FileTree(FolderTree* parent, const wchar_t* name, int codePage)
    : m_path()
    , m_name()
    , m_time()
{
    init();
    m_parent = parent;
    m_size = 0;
    m_codePage = codePage;
    if (name)
    {
        const wchar_t* fileName = name;
        if (wcschr(name, L'\\'))
        {
            m_path = name;
            fileName = PathFindFileNameW(name);
        }
        m_name = fileName;
    }
}

FileTree::~FileTree()
{
}

void FileTree::init()
{
    m_attr = 0;
    m_offset = 0;
    m_size = 0;
    m_time.m_year = 0;
    m_time.m_month = 0;
    m_time.m_day = 0;
    m_time.m_minute = 0;
    m_time.m_second = 0;
    m_time.m_hour = 0;
    m_time.m_tzOffset = 0;
    m_time.m_fullYear = true;
    m_time.m_valid = false;
    m_state = 0;
    m_parent = 0;
    m_codePage = 0;
    m_icon = -1;
    memset(m_key, 0, sizeof(m_key));
}

int FileTree::getId() const
{
    return m_icon;
}

int FileTree::displayV2() const
{
    return m_state;
}

const wchar_t* FileTree::getString(DWORD tag) const
{
    switch (tag)
    {
    case 'path':
        return m_path.m_buffer;
    case 'name':
        return m_name.m_buffer;
    case 'time':
    {
        String text = const_cast<Time&>(m_time).format();
        memset(g_fileTimeText, 0, sizeof(g_fileTimeText));
        memcpy(g_fileTimeText, text.m_buffer, 2 * text.m_length);
        return g_fileTimeText;
    }
    case 'stat':
        if (m_state & STATE_DIRTY)
            return L"dirty";
        if (m_state & STATE_LOADED)
            return L"loaded";
        if (m_state & STATE_NEWER)
            return L"newer";
        return L"";
    default:
        return 0;
    }
}

int FileTree::getInt(DWORD tag) const
{
    switch (tag)
    {
    case '1col':
        return 0;
    case 'attr':
        return (int)m_attr;
    case 'offs':
        return (int)m_offset;
    case 'size':
        return (int)m_size;
    case 'icon':
        return m_icon;
    case 'cdpg':
        return m_codePage;
    default:
        return 0;
    }
}

bool FileTree::nodeV6(int tag)
{
    if (tag == 'cdpg')
    {
        m_codePage = tag;
        return true;
    }
    if (tag != 'time')
        return false;
    if (m_path.m_length && m_path.m_buffer[0] == L'\\')
        return false;

    bool ok = false;
    HANDLE file = CreateFileW(m_path.m_buffer, FILE_READ_ATTRIBUTES, FILE_SHARE_READ, 0, OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL, 0);
    if (file != INVALID_HANDLE_VALUE)
    {
        FILETIME lastWrite;
        lastWrite.dwLowDateTime = 0;
        lastWrite.dwHighDateTime = 0;
        if (GetFileTime(file, 0, 0, &lastWrite))
        {
            SYSTEMTIME st;
            FileTimeToLocalFileTime(&lastWrite, &lastWrite);
            FileTimeToSystemTime(&lastWrite, &st);
            m_time = st;
            ok = true;
        }
        CloseHandle(file);
    }
    return ok;
}

void FileTree::reset()
{
    m_path.clear();
    m_name.clear();
    m_time.m_year = 0;
    m_time.m_month = 0;
    m_time.m_day = 0;
    m_time.m_minute = 0;
    m_time.m_second = 0;
    m_time.m_hour = 0;
    m_time.m_tzOffset = 0;
    m_time.m_fullYear = true;
    m_time.m_valid = false;
    m_attr = 0;
    m_offset = 0;
    m_size = 0;
    m_icon = 0;
    m_state = 0;
    m_codePage = 0;
}

int FileTree::compare(const Node* otherNode, DWORD tag) const
{
    const FileTree* other = dynamic_cast<const FileTree*>(otherNode);
    const wchar_t* a;
    const wchar_t* b;
    switch (tag)
    {
    case 'path':
        a = m_path.m_buffer;
        b = other->m_path.m_buffer;
        break;
    case 'name':
        a = m_name.m_buffer;
        b = other->m_name.m_buffer;
        break;
    case 'attr':
        if (m_attr == other->m_attr)
            return 0;
        return m_attr < other->m_attr ? -1 : 1;
    case 'offs':
        return compareInt64(m_offset, other->m_offset);
    case 'size':
        return compareInt64(m_size, other->m_size);
    case 'icon':
        if (m_icon == other->m_icon)
            return 0;
        return m_icon >= other->m_icon ? 1 : -1;
    case 'stat':
        if (m_state == other->m_state)
            return 0;
        return m_state >= other->m_state ? 1 : -1;
    case 'time':
        return m_time.compare(other->m_time);
    default:
        return 0;
    }
    if (!a)
        return -(b != 0);
    if (!b)
        return 1;
    return StrCmpLogicalW(a, b);
}

void FileTree::read(iStream& in, DWORD fmt)
{
    reset();
    m_name.read(in, 4, static_cast<aStream&>(in).m_codePage);
    m_offset = in.readInt64();
    m_size = (__int64)(unsigned __int64)in.readInt();
    m_attr = in.readInt();
}

void FileTree::write(oStream& out, DWORD fmt) const
{
    m_name.write(out, 4, static_cast<aStream&>(out).m_codePage);
    out << m_offset << (unsigned long)(DWORD)m_size << (unsigned long)m_attr;
}

bool FileTree::toText(String& out, DWORD fmt) const
{
    String* line;
    if (fmt == 'dump')
    {
        out.append(m_name.m_buffer, m_name.m_length) << L'\t' << m_offset << L'\t';
        line = &(out << m_size);
    }
    else
    {
        out << L'"';
        out.append(m_name.m_buffer, m_name.m_length) << L'"' << L';' << m_offset << L';';
        out << m_size << L';';
        line = &(out << (int)m_attr);
    }
    *line << L"\r\n";
    return true;
}

int FileTree::nodeV11(int hTreeArg)
{
    HWND hTree = (HWND)hTreeArg;
    IDataObject* result = 0;
    if (PathIsRelativeW(m_path.m_buffer))
    {
        Bytes data(0);
        if (!g_contentProvider || g_contentProvider->readFile(this, &data, false))
            return 0;

        FORMATETC formats[3];
        formats[0].cfFormat = (CLIPFORMAT)g_cfWorldFile;
        formats[0].ptd = 0;
        formats[0].dwAspect = DVASPECT_CONTENT;
        formats[0].lindex = 0;
        formats[0].tymed = TYMED_HGLOBAL;
        formats[1].cfFormat = (CLIPFORMAT)g_cfFileContents;
        formats[1].ptd = 0;
        formats[1].dwAspect = DVASPECT_CONTENT;
        formats[1].lindex = 0;
        formats[1].tymed = TYMED_HGLOBAL;
        formats[2].cfFormat = (CLIPFORMAT)g_cfFileGroupDescriptorW;
        formats[2].ptd = 0;
        formats[2].dwAspect = DVASPECT_CONTENT;
        formats[2].lindex = 0;
        formats[2].tymed = TYMED_HGLOBAL;

        STGMEDIUM media[3];
        media[0].tymed = TYMED_HGLOBAL;
        media[0].pUnkForRelease = 0;
        media[1].tymed = TYMED_HGLOBAL;
        media[1].pUnkForRelease = 0;
        media[2].tymed = TYMED_HGLOBAL;
        media[2].pUnkForRelease = 0;
        media[0].hGlobal = m_path.toHGlobal();
        media[1].hGlobal = data.toHGlobal();

        HGLOBAL hDesc = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, sizeof(FILEGROUPDESCRIPTORW));
        media[2].hGlobal = hDesc;
        FILEGROUPDESCRIPTORW* desc = (FILEGROUPDESCRIPTORW*)GlobalLock(hDesc);
        desc->cItems = 1;
        desc->fgd[0].dwFlags = FD_ATTRIBUTES | FD_FILESIZE;
        desc->fgd[0].nFileSizeLow = static_cast<aStream&>(data).m_size;
        desc->fgd[0].dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
        memcpy(desc->fgd[0].cFileName, m_name.m_buffer, 2 * m_name.m_length);
        GlobalUnlock(hDesc);

        result = new DataObject(formats, media, 3);
        return (int)result;
    }

    if (getFileDataObject(GetParent(hTree), m_path.m_buffer, (void**)&result) >= 0)
    {
        SHDRAGIMAGE drag;
        memset(&drag, 0, sizeof(drag));
        HIMAGELIST himl = (HIMAGELIST)SendMessageW(hTree, TVM_GETIMAGELIST, 0, 0);
        if (createDragImage(himl, m_icon, &drag))
            initializeDragImage(&drag, &result);
    }
    return (int)result;
}

bool FileTree::nodeV12(int hTreeArg)
{
    if (!m_parent || !m_parent->m_files.detachItem(this))
        return false;
    SendMessageW((HWND)hTreeArg, TVM_DELETEITEM, 0, (LPARAM)m_data);
    return true;
}
