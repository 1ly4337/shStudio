#include "kWorld_Element.h"
#include "Bytes.h"
#include "DataObject.h"
#include "dragdrop.h"
#include <commctrl.h>
#include "String_p02.h"
#include "CapeData.h"

namespace kWorld
{

    Element::Element(int sectionIndex, int mode)
        : Section(sectionIndex)
        , m_items(mode, this)
    {
    }

    bool Element::hasItem(const wchar_t* name)
    {
        return m_items.hasItem(name);
    }

    Node* Element::addItem(const wchar_t* name)
    {
        Item* item = new Item(m_items.m_count, m_items.m_owner, m_items.m_mode);
        item->setString('name', name);
        m_items.add(item);
        return item;
    }

    bool Element::removeItem(Node* node)
    {
        Item* item = dynamic_cast<Item*>(node);
        if (!m_items.detachItem(item))
            return false;
        m_items.destroyItem(item);
        return true;
    }

    void Element::read(iStream& in, DWORD fmt)
    {
        m_items.read(in, fmt);
    }

    void Element::write(oStream& out, DWORD fmt) const
    {
        m_items.write(out, fmt);
    }

    bool Element::toText(String& out, DWORD fmt) const
    {
        if (fmt == '----' || fmt == 'dump')
        {
            if (m_index >= 0 && m_index <= 10)
                out << g_sectionCaptions[m_index] << L"\r\n";

            int count = m_items.size();
            for (int i = 0; i < count; ++i)
            {
                Item* item = m_items.get(i);
                const wchar_t* name = FIELD(const wchar_t*, item, 0x18);
                out << L'\t';
                String& line = out << (long)FIELD(int, item, 0xC);
                line << L':';
                line << L'\t';
                line << name;
                line.newline();

                int coords = item->m_coords.size();
                for (int j = 0; j < coords; ++j)
                {
                    Coord* coord = item->m_coords.m_items[j];
                    const Streamable* s = coord ? static_cast<const Streamable*>(coord) : 0;
                    out << L'\t';
                    out << L'\t';
                    s->toText(out, '----');
                    out.newline();
                }
            }
            return true;
        }

        int count = m_items.size();
        out << "Files (" << (long)count << " items)" << L"\r\n";
        for (int i = 0; i < count; ++i)
        {
            Item* item = m_items.get(i);
            const wchar_t* name = FIELD(const wchar_t*, item, 0x18);
            out << L'\t';
            String& line = out << (long)FIELD(int, item, 0xC);
            line << L':';
            line << L'\t';
            line << name << L"\r\n";
        }
        return true;
    }

    bool Element::Items::hasItem(const wchar_t* name) const
    {
        if (!name)
            return false;
        String lower;
        lower.append(name, (unsigned)-1);
        lower.toLowerInPlace();
        const wchar_t* text = lower.m_buffer;
        for (int i = 0; i < m_count; ++i)
        {
            if (text && FIELD(String, m_items[i], 0x14).equalsLowerCase(text, (unsigned)wcslen(text)))
                return true;
        }
        return false;
    }

    void Element::Items::read(iStream& in, DWORD fmt)
    {
        deleteAll();
        if (in.m_size - in.m_pos < 4)
            throw 1;
        int count = (int)in.readInt();
        if (count < 0 || count > (in.m_size - in.m_pos) / 256)
            throw 2;
        for (int i = 0; i < count; ++i)
        {
            Item* item = new Item(i, m_owner, m_mode);
            static_cast<Streamable*>(item)->read(in, fmt);
            add(item);
        }

        if (in.m_size - in.m_pos < 4)
            throw 0;
        int coords = (int)in.readInt();
        for (int i = 0; i < coords; ++i)
        {
            Coord* coord = new Coord(m_mode);
            static_cast<Streamable*>(coord)->read(in, fmt);
            m_items[coord->m_itemIndex]->m_coords.add(coord);
        }
    }

    void Element::Items::write(oStream& out, DWORD fmt) const
    {
        int total = 0;
        out << (unsigned long)m_count;
        for (int i = 0; i < m_count; ++i)
        {
            Item* item = m_items[i];
            static_cast<const Streamable*>(item)->write(out, fmt);
            total += item->m_coords.size();
        }
        out << (unsigned long)total;
        for (int i = 0; i < m_count; ++i)
        {
            Item* item = m_items[i];
            int n = item->m_coords.size();
            for (int j = 0; j < n; ++j)
            {
                Coord* coord = item->m_coords.m_items[j];
                coord->m_itemIndex = i;
                static_cast<const Streamable*>(coord)->write(out, fmt);
            }
        }
    }

    bool Element::Coords::add(Element::Coord* coord)
    {
        if (!coord)
            return false;
        if (m_count == m_capacity && !grow(m_capacity + m_growBy))
            return false;
        m_items[m_count++] = coord;
        coord->m_index = m_count;
        return true;
    }

    Element::Item::Item(int index, Element* owner, int mode)
        : Section::wItem(index, owner)
        , m_coords()
        , m_mode(mode)
    {
    }

    void Element::Item::reset()
    {
        m_coords.deleteAll();
        String& name = FIELD(String, this, 0x14);
        wchar_t* buf = name.m_buffer;
        name.m_length = 0;
        name.m_capacity = 0;
        if (buf)
        {
            operator delete[](buf);
            name.m_buffer = 0;
        }
        String& second = FIELD(String, this, 0x34);
        buf = second.m_buffer;
        second.m_length = 0;
        second.m_capacity = 0;
        if (buf)
        {
            operator delete[](buf);
            second.m_buffer = 0;
        }
    }

    int Element::Item::nodeV11(int hTreeArg)
    {
        HWND hTree = (HWND)hTreeArg;
        TVITEMW tvi;
        memset(&tvi, 0, sizeof(tvi));
        tvi.hItem = (HTREEITEM)m_data;
        tvi.mask = TVIF_IMAGE;
        if (!SendMessageW(hTree, TVM_GETITEMW, 0, (LPARAM)&tvi))
            return 0;
        HTREEITEM hItem = (HTREEITEM)m_data;
        int image = tvi.iImage;
        tvi.mask = TVIF_PARAM | TVIF_HANDLE;
        tvi.hItem = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM, TVGN_PARENT, (LPARAM)hItem);
        if (!SendMessageW(hTree, TVM_GETITEMW, 0, (LPARAM)&tvi))
            return 0;
        Node* parent = tvi.lParam ? *(Node**)(tvi.lParam + 4) : 0;
        Element* element = dynamic_cast<Element*>(parent);
        if (!element)
            return 0;

        String path;
        if (element->m_index == 6)
        {
            path << "world\\dungeon";
        }
        else
        {
            path << "Entity\\";
            path << (element->m_index == 5 ? g_sectionNames[4] : g_sectionNames[element->m_index]);
        }
        path << L'\\';
        path.append(FIELD(const wchar_t*, this, 0x18), FIELD(unsigned, this, 0x1C));

        IDataObject* result = 0;
        void* frame = g_studioFrame;
        if (P04_VCALL(frame, 0x170, bool)(frame))
        {
            Bytes data(0);
            if (P04_VCALL(frame, 0x14C, int, Bytes*, String*, int)(frame, &data, &path, 0))
                return 0;

            FORMATETC formats[2];
            formats[0].cfFormat = (CLIPFORMAT)g_cfFileContents;
            formats[0].ptd = 0;
            formats[0].dwAspect = DVASPECT_CONTENT;
            formats[0].lindex = 0;
            formats[0].tymed = TYMED_HGLOBAL;
            formats[1].cfFormat = (CLIPFORMAT)g_cfFileGroupDescriptorW;
            formats[1].ptd = 0;
            formats[1].dwAspect = DVASPECT_CONTENT;
            formats[1].lindex = 0;
            formats[1].tymed = TYMED_HGLOBAL;

            STGMEDIUM media[2];
            media[0].tymed = TYMED_HGLOBAL;
            media[0].pUnkForRelease = 0;
            media[1].tymed = TYMED_HGLOBAL;
            media[1].pUnkForRelease = 0;
            media[0].hGlobal = data.toHGlobal();

            HGLOBAL hDesc = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, sizeof(FILEGROUPDESCRIPTORW));
            media[1].hGlobal = hDesc;
            FILEGROUPDESCRIPTORW* desc = (FILEGROUPDESCRIPTORW*)GlobalLock(hDesc);
            desc->cItems = 1;
            desc->fgd[0].dwFlags = FD_ATTRIBUTES | FD_FILESIZE;
            desc->fgd[0].nFileSizeLow = data.m_size;
            desc->fgd[0].dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
            memcpy(desc->fgd[0].cFileName, FIELD(const wchar_t*, this, 0x18), 2 * FIELD(unsigned, this, 0x1C));
            GlobalUnlock(hDesc);

            result = new DataObject(formats, media, 2);
            return (int)result;
        }

        String full;
        P04_VCALL(frame, 0x158, String*, String*)(frame, &full);
        full.append(path.m_buffer, path.m_length);
        if (getFileDataObject(GetParent(hTree), full.m_buffer, (void**)&result) >= 0)
        {
            SHDRAGIMAGE drag;
            memset(&drag, 0, sizeof(drag));
            HIMAGELIST himl = (HIMAGELIST)SendMessageW(hTree, TVM_GETIMAGELIST, 0, 0);
            if (createDragImage(himl, image, &drag))
                initializeDragImage(&drag, &result);
        }
        return (int)result;
    }

    bool Element::Item::nodeV12(int hTreeArg)
    {
        HWND hTree = (HWND)hTreeArg;
        TVITEMW tvi;
        memset(&tvi, 0, sizeof(tvi));
        tvi.mask = TVIF_PARAM | TVIF_HANDLE;
        tvi.hItem = (HTREEITEM)SendMessageW(hTree, TVM_GETNEXTITEM, TVGN_PARENT, (LPARAM)m_data);
        if (!SendMessageW(hTree, TVM_GETITEMW, 0, (LPARAM)&tvi))
            return false;
        Node* parent = tvi.lParam ? *(Node**)(tvi.lParam + 4) : 0;
        Element* element = dynamic_cast<Element*>(parent);
        if (!element)
            return false;
        HTREEITEM hItem = (HTREEITEM)m_data;
        if (!element->removeItem(this))
            return false;
        SendMessageW(hTree, TVM_DELETEITEM, 0, (LPARAM)hItem);
        return true;
    }

    bool Element::Item::removeCoord(Element::Coord* coord)
    {
        if (!m_coords.detachItem(coord))
            return false;
        m_coords.destroyItem(coord);
        return true;
    }

    Element::Coord::Coord(int mode)
        : Node()
        , m_pos()
        , m_rot()
        , m_scale()
    {
        m_index = -1;
        m_mode = mode;
        m_unknown = 0;
        m_itemIndex = 0;
        m_scale.z = 0;
        m_scale.y = 0;
        m_scale.x = 0;
        m_rot.x = 0;
        m_rot.y = m_scale.y;
        m_rot.z = m_scale.z;
        m_pos.x = 0;
        m_pos.y = m_rot.y;
        m_pos.z = m_rot.z;
    }

    short Element::Coord::getImage(int tag, short defImage) const
    {
        if (tag > 'x3__')
        {
            if (tag == 'y3__' || tag == 'z3__')
                return 130;
        }
        else
        {
            if (tag == 'x3__')
                return 130;
            if (tag > 'rotZ')
            {
                if (tag == 'unkn')
                    return 18;
            }
            else if (tag >= 'rotX' || (tag >= 'posX' && tag <= 'posZ'))
            {
                return 130;
            }
        }
        return defImage;
    }

    const wchar_t* Element::Coord::getString(DWORD tag) const
    {
        if (tag != 'name')
            return 0;
        sub_43CBF0(g_textBuffer, (wchar_t*)L"#%d", m_index);
        return g_textBuffer;
    }

    int Element::Coord::getInt(DWORD tag) const
    {
        switch (tag)
        {
        case 'indx': return m_index;
        case 'idnt': return m_itemIndex;
        case 'unkn': return m_unknown;
        case 'posX': return *(const int*)&m_pos.x;
        case 'posY': return *(const int*)&m_pos.y;
        case 'posZ': return *(const int*)&m_pos.z;
        case 'rotX': return *(const int*)&m_rot.x;
        case 'rotY': return *(const int*)&m_rot.y;
        case 'rotZ': return *(const int*)&m_rot.z;
        case 'x3__': return *(const int*)&m_scale.x;
        case 'y3__': return *(const int*)&m_scale.y;
        case 'z3__': return *(const int*)&m_scale.z;
        }
        return 0;
    }

    void Element::Coord::reset()
    {
        m_unknown = 0;
        m_itemIndex = 0;
        m_scale.z = 0;
        m_scale.y = 0;
        m_scale.x = 0;
        m_rot.x = 0;
        m_rot.y = m_scale.y;
        m_rot.z = m_scale.z;
        m_pos.x = 0;
        m_pos.y = m_rot.y;
        m_pos.z = m_rot.z;
    }

    void Element::Coord::assignFrom(const Coord* src)
    {
        m_mode = src->m_mode;
        m_unknown = src->m_unknown;
        m_itemIndex = src->m_itemIndex;
        m_pos.x = src->m_pos.x;
        m_pos.y = src->m_pos.y;
        m_pos.z = src->m_pos.z;
        m_rot.x = src->m_rot.x;
        m_rot.y = src->m_rot.y;
        m_rot.z = src->m_rot.z;
        m_scale.x = src->m_scale.x;
        m_scale.y = src->m_scale.y;
        m_scale.z = src->m_scale.z;
    }

    void Element::Coord::read(iStream& in, DWORD fmt)
    {
        if (m_mode == 2 || fmt == 'clip')
            m_unknown = (int)in.readInt();
        m_itemIndex = (int)in.readInt();
        m_pos.read(in, 'file');
        m_rot.read(in, 'file');
        m_scale.read(in, 'file');
    }

    void Element::Coord::write(oStream& out, DWORD fmt) const
    {
        if (m_mode == 2 || fmt == 'clip')
            out << (unsigned long)m_unknown;
        oStream& o = out << (unsigned long)m_itemIndex;
        m_pos.write(o, 'file');
        m_rot.write(o, 'file');
        m_scale.write(o, 'file');
    }

    bool Element::Coord::toText(String& out, DWORD fmt) const
    {
        wchar_t sep = p04_separator(fmt);
        if (m_mode == 2)
        {
            String& s = out << (long)m_unknown;
            s << sep;
        }
        String& s1 = out << (long)m_itemIndex;
        s1 << sep;
        String& s2 = appendFPoint(s1, &m_pos);
        s2 << sep;
        String& s3 = appendFPoint(s2, &m_rot);
        s3 << sep;
        appendFPoint(s3, &m_scale);
        return true;
    }

    bool Element::Coord::copyFrom(const Streamable* src)
    {
        const Coord* coord = dynamic_cast<const Coord*>(src);
        if (!coord)
            return false;
        assignFrom(coord);
        return true;
    }
}
