#pragma once
#include <windows.h>

class aStream;

class crypto_exception {
public:
    virtual ~crypto_exception() {}
};

class AES {
public:
    explicit AES(int keyBits) : m_keyBits(keyBits) {}
    virtual ~AES() {}
    virtual void unsupported1() {}
    virtual void unsupported2() {}
    virtual void unsupported3() {}
    virtual void unsupported4() {}
    virtual void unsupported5() {}
    virtual void transform(WCHAR* , int , WCHAR* ) {}
    virtual void unsupported7() {}

    bool setKey(const void* , int ) { return false; }
    bool decrypt(const void* , int , void* ) { return false; }

private:
    int m_keyBits;
};

bool __fastcall sub_418C00(aStream* data, void* unused, void* key);
bool __fastcall sub_418E60(aStream* data, void* header);
