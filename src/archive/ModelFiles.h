#pragma once
#include <windows.h>
#include <cwchar>
#include "ptrArray.h"
#include "iStream.hpp"
#include "TreeList.h"
#include "ContentProvider.h"
#include "ida_compat.h"

class FileTree;

template <class T>
struct ModelFile
{
    FileTree* node;
    T*        data;
};

template <class T>
class ModelFiles : public ptrArray<ModelFile<T>*>
{
public:
    ModelFiles() : ptrArray<ModelFile<T>*>(256) {}
    virtual ~ModelFiles() {}

    virtual void destroyItem(ModelFile<T>* item) override
    {
        if (item)
            ::operator delete(item);
    }

    T* find(const wchar_t* name)
    {
        int i = 0;
        if (this->m_count <= 0)
            return 0;
        while (true)
        {
            ModelFile<T>* entry = this->m_items[i];
            FileTree* node = entry->node;
            if (node && name)
            {
                const wchar_t* nodeName = FIELD(const wchar_t*, node, 0x34);
                unsigned int nodeLen = FIELD(unsigned int, node, 0x38);
                if (!*name)
                {
                    if (nodeLen == 0)
                        return entry->data;
                }
                else if (wcslen(name) == nodeLen)
                {
                    if (_wcsnicmp(nodeName, name, nodeLen) == 0)
                        return entry->data;
                }
            }
            if (++i >= this->m_count)
                return 0;
        }
    }

    T* load(FileTree* node, TreeList* tree)
    {
        T* found = find(FIELD(const wchar_t*, node, 0x34));
        if (found)
            return found;
        iStream in;
        int err;
        if (g_contentProvider)
            err = g_contentProvider->readFile(node, &in, false);
        else
            err = in.aStream::load(FIELD(const wchar_t*, node, 0x14));
        if (!err)
        {
            T* obj = new T();
            static_cast<Streamable*>(obj)->read(in, 'file');
            FIELD(int, node, 0xFC) = 1;
            tree->invalidateItem(FIELD(int, node, 0x08));
            ModelFile<T>* entry = new ModelFile<T>;
            if (entry)
            {
                entry->node = node;
                entry->data = obj;
            }
            this->add(entry);
            return obj;
        }
        return 0;
    }
};
