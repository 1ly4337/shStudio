#pragma once
#include <windows.h>
#include <cstddef>
#include "Node.h"
#include "String.hpp"
#include "Time.hpp"

class FolderTree;
class iStream;
class oStream;

#pragma warning(disable : 4250)
#pragma vtordisp(push, 2)

class FileTree : public Node
{
public:
    enum State
    {
        STATE_LOADED = 0x01,
        STATE_DIRTY  = 0x02,
        STATE_NEWER  = 0x10,
    };

    FileTree();
    FileTree(FolderTree* parent, const wchar_t* name, int codePage = 0);
    FileTree(FolderTree* parent, const wchar_t* name, int unused1, int unused2, int codePage, int unused3)
        : FileTree(parent, name, codePage) {}
    virtual ~FileTree();

    virtual int getId() const override;
    virtual int displayV2() const override;
    virtual const wchar_t* getString(DWORD tag) const override;
    virtual int getInt(DWORD tag) const override;
    virtual bool nodeV6(int tag) override;
    virtual void reset() override;
    virtual int compare(const Node* other, DWORD tag) const override;
    virtual int nodeV11(int hTree) override;
    virtual bool nodeV12(int hTree) override;

    virtual void read(iStream& in, DWORD fmt) override;
    virtual void write(oStream& out, DWORD fmt) const override;
    virtual bool toText(String& out, DWORD fmt) const override;

    void init();

public:
    FolderTree* m_parent;
    String      m_path;
    String      m_name;
    DWORD       m_attr;
    int         m_pad54;
    __int64     m_offset;
    __int64     m_size;
    Time        m_time;
    int         m_icon;
    int         m_state;
    BYTE        m_key[64];
    int         m_codePage;
    int         m_pad144;
};

#pragma vtordisp(pop)

static_assert(offsetof(FileTree, m_parent) == 0x0C, "FileTree::m_parent");
static_assert(offsetof(FileTree, m_path) == 0x10, "FileTree::m_path");
static_assert(offsetof(FileTree, m_name) == 0x30, "FileTree::m_name");
static_assert(offsetof(FileTree, m_offset) == 0x58, "FileTree::m_offset");
static_assert(offsetof(FileTree, m_time) == 0x68, "FileTree::m_time");
static_assert(offsetof(FileTree, m_icon) == 0xF8, "FileTree::m_icon");
static_assert(offsetof(FileTree, m_key) == 0x100, "FileTree::m_key");
static_assert(offsetof(FileTree, m_codePage) == 0x140, "FileTree::m_codePage");
static_assert(sizeof(FileTree) == 0x150, "FileTree size");
