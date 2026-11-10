#pragma once
#include <windows.h>
#include <GL/gl.h>
#include "Drawable.h"
#include "ptrArray.h"
#include "VaniSubStruct.h"

class iStream;
class ListCtrl;

class Vani : public Drawable
{
public:
    Vani();
    virtual ~Vani();

    virtual void clear() override;
    virtual void translate(const fPoint* origin) override {}
    virtual void computeBounds() override;
    virtual void fillProperties(ListCtrl* list) override;
    virtual void drawWireframe(GLenum front, GLenum back) override;
    virtual void drawImmediate() override;
    virtual bool loadTexture(const wchar_t* dir) override;
    virtual bool isEmpty();

    void draw(bool textured);
    void read(iStream& in);

    float m_rotation;
    int   m_nSubItems;
    int   m_nSteps;
    int   m_unk74;
    ptrArray<VaniSubStruct*> m_subs;
    int   m_curStep;
};
static_assert(sizeof(Vani) == 0x94, "Vani size");
