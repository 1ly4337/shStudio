#pragma once
#include <windows.h>

class String;

class RecordSet
{
public:
    class iterator
    {
    public:
        virtual ~iterator() {}
        virtual bool atEnd() const = 0;
        virtual int  getRow() const = 0;
        virtual bool next() = 0;
        virtual const wchar_t* getString(const wchar_t* column) = 0;
        virtual const wchar_t* getStringAt(int index) = 0;
        virtual int  getInt(const wchar_t* column) = 0;
        virtual int  getIntAt(int index) = 0;
        virtual double getDouble(const wchar_t* column) = 0;
        virtual double getDoubleAt(int index) = 0;
        virtual SYSTEMTIME getTimeAt(int index) = 0;
        virtual bool hasColumn(const wchar_t* column) = 0;
        virtual void vslot12(String& out) = 0;

        const wchar_t* getString(int index) { return getStringAt(index); }
        int    getInt(int index)            { return getIntAt(index); }
        double getDouble(int index)         { return getDoubleAt(index); }
    };
    static_assert(sizeof(iterator) == 4, "RecordSet::iterator size");

    class _RecordSet
    {
    public:
        _RecordSet() : m_refCount(0) {}
        virtual ~_RecordSet() {}
        virtual iterator* getIterator() = 0;
        virtual int getCount() const = 0;
        virtual int getColumnCount() const = 0;

        int m_refCount;
    };
    static_assert(sizeof(_RecordSet) == 8, "RecordSet::_RecordSet size");

    explicit RecordSet(_RecordSet* rs = 0) : m_rs(rs)
    {
        if (m_rs)
            ++m_rs->m_refCount;
    }
    RecordSet(const RecordSet& other) : m_rs(other.m_rs)
    {
        if (m_rs)
            ++m_rs->m_refCount;
    }
    RecordSet& operator=(const RecordSet& other)
    {
        if (other.m_rs)
            ++other.m_rs->m_refCount;
        release();
        m_rs = other.m_rs;
        return *this;
    }
    virtual ~RecordSet();

    bool next();

    const wchar_t* getString(const wchar_t* column) { return m_rs ? m_rs->getIterator()->getString(column) : 0; }
    const wchar_t* getString(int index)             { return m_rs ? m_rs->getIterator()->getStringAt(index) : 0; }
    int    getInt(const wchar_t* column)            { return m_rs ? m_rs->getIterator()->getInt(column) : 0; }
    int    getInt(int index)                        { return m_rs ? m_rs->getIterator()->getIntAt(index) : 0; }
    double getDouble(const wchar_t* column)         { return m_rs ? m_rs->getIterator()->getDouble(column) : 0.0; }
    double getDouble(int index)                     { return m_rs ? m_rs->getIterator()->getDoubleAt(index) : 0.0; }
    SYSTEMTIME getTime(int index)
    {
        if (m_rs)
            return m_rs->getIterator()->getTimeAt(index);
        SYSTEMTIME none = {};
        return none;
    }
    bool hasColumn(const wchar_t* column)           { return m_rs && m_rs->getIterator()->hasColumn(column); }

    union
    {
        _RecordSet* m_rs;
        _RecordSet* m_impl;
    };

private:
    void release()
    {
        if (m_rs)
        {
            if (m_rs->m_refCount > 0)
                --m_rs->m_refCount;
            if (!m_rs->m_refCount)
                delete m_rs;
        }
    }
};
static_assert(sizeof(RecordSet) == 8, "RecordSet size");
