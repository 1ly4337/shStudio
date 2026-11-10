#pragma once
#include "ptrArray.h"
#include "primitiveArray.h"
#include "String.hpp"

class Strings : public ptrArray<String*>
{
public:
    class compare_string
    {
    public:
        virtual int operator()(const String* a, const String* b) const;
    };

    explicit Strings(int capacity = 16) : ptrArray<String*>(capacity), m_allowDuplicates(false) {}
    virtual ~Strings() {}

    virtual int indexOf(const wchar_t* text) const;
    virtual bool add(long id, const wchar_t* text);
    virtual bool add(const wchar_t* text);
    using ptrArray<String*>::add;
    using ptrArray<String*>::indexOf;

    Strings& operator=(const Strings& other);
    String* at(int index) const;
    String* findById(long id) const;
    String* strID(long id) const;
    void sort(int first, int last, bool logical);

    bool m_allowDuplicates;
    primitiveArray<long> m_ids;
};
