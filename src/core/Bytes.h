#pragma once
#include "iStream.hpp"
#include "oStream.hpp"

class Bytes : public iStream, public oStream
{
public:
    explicit Bytes(int unused = 0, UINT codePage = 0);
    Bytes(const void* data, int size, int unused = 0);
    Bytes(const Bytes& other);
    virtual ~Bytes();
    virtual int load(const wchar_t* fileName) override;
    virtual int save(const wchar_t* fileName) override;

    Bytes& operator=(const Bytes& other);
    void   release();
    HGLOBAL toHGlobal() const;
    Bytes& append(const Bytes& other);
};
