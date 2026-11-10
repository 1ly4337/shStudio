#pragma once
#include "Streamable.h"

class iStream;

#pragma vtordisp(push, 0)
class Serializable : public virtual Streamable
{
public:
    Serializable() : m_buffer1(0), m_buffer2(0), m_fileName(0) {}
    virtual ~Serializable();

    virtual bool isEmpty() const = 0;
    virtual void clear() = 0;
    virtual int getCodePage() const;
    virtual const wchar_t* getFilePrefix() const { return 0; }
    virtual void load(iStream& in, int arg2, int arg3) {}
    virtual int serialV5() const { return -1; }
    virtual HRESULT setCodePage(int codePage) { return E_NOTIMPL; }

    void setFileName(const wchar_t* fileName);
    static void checkSignature(iStream& in, const wchar_t* expected);

    wchar_t* m_buffer1;
    wchar_t* m_buffer2;
    wchar_t* m_fileName;
};
#pragma vtordisp(pop)
