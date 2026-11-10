#include "p05_common.h"
#include "Doc.h"

void Doc::onNcDestroy()
{
    if (this)
        VCALL(this, 0, void, int)(this, 1);
}

LPCWSTR Doc::getName()
{
    void* node = m_data;
    if (node)
        return FIELD(const wchar_t*, node, 0x34);
    return L"noname";
}

bool Doc::isPath(const wchar_t* path)
{
    void* node = m_data;
    if (!path || !*path)
        return FIELD(int, node, 0x18) == 0;
    if (wcslen(path) == FIELD(size_t, node, 0x18))
        return _wcsnicmp(FIELD(const wchar_t*, node, 0x14), path, FIELD(size_t, node, 0x18)) == 0;
    return false;
}

int Doc::getType()
{
    void* node = m_data;
    if (node)
        return FIELD(int, node, 0x140);
    return 0;
}

LPCWSTR Doc::getPath()
{
    return FIELD(const wchar_t*, m_data, 0x14);
}

bool Doc::isModified()
{
    return m_modified;
}

SIZE* Doc::getDefaultSize(SIZE* out, SIZE def)
{
    *out = def;
    return out;
}

HRESULT Doc::load()
{
    return E_NOTIMPL;
}

HRESULT Doc::store()
{
    return E_NOTIMPL;
}
