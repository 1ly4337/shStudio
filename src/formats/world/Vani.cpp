#include "Vani.h"
#include "p04_common.h"
#include "iStream.h"
#include "ListCtrl.h"
#include <cfloat>

Vani::Vani()
    : Drawable(), m_subs()
{
    m_rotation = 0;
    m_nSubItems = 0;
    m_nSteps = 0;
    m_unk74 = 0;
    m_curStep = 0;
}

Vani::~Vani()
{
    clear();
}

void Vani::clear()
{
    Drawable::clear();
    m_subs.deleteAll();
}

void Vani::computeBounds()
{
    m_min.z = FLT_MAX;
    m_min.y = FLT_MAX;
    m_min.x = FLT_MAX;
    m_max.z = -FLT_MAX;
    m_max.y = -FLT_MAX;
    m_max.x = -FLT_MAX;
    int count = m_subs.size();
    for (int i = 0; i < count; ++i)
    {
        VaniSubStruct* sub = m_subs.m_items[i];
        sub->computeBounds();
        const fPoint& smin = sub->m_min;
        const fPoint& smax = sub->m_max;
        if (m_min.x > smin.x)
            m_min.x = smin.x;
        if (m_min.y > smin.y)
            m_min.y = smin.y;
        if (m_min.z > smin.z)
            m_min.z = smin.z;
        if (smax.x > m_max.x)
            m_max.x = smax.x;
        if (smax.y > m_max.y)
            m_max.y = smax.y;
        if (smax.z > m_max.z)
            m_max.z = smax.z;
    }
    m_center.x = (m_max.x + m_min.x) * 0.5f;
    m_center.y = (m_max.y + m_min.y) * 0.5f;
    m_center.z = (m_max.z + m_min.z) * 0.5f;
}

void Vani::fillProperties(ListCtrl* list)
{
    list->insertNumberRow(L"Number of animation steps", m_nSteps, 0);
    list->insertNumberRow(L"Number of 3D sub-items", m_nSubItems, 0);
}

bool Vani::isEmpty()
{
    return m_subs.size() == 0;
}

void Vani::drawWireframe(GLenum front, GLenum back)
{
    draw(false);
}

void Vani::drawImmediate()
{
    draw(true);
}

void Vani::draw(bool textured)
{
    int count = m_subs.size();
    if (!count)
        return;
    int step = ++m_curStep;
    if (step >= m_nSteps)
        step = 0;
    m_curStep = step;
    for (int i = 0; i < count; ++i)
    {
        VaniSubStruct* sub = m_subs.m_items[i];
        sub->m_curStep = m_curStep;
        sub->setFlag(1, textured);
    }
    if (textured)
    {
        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.25f);
    }
    else
    {
        glDisable(GL_TEXTURE_2D);
        glPolygonMode(GL_FRONT, GL_LINE);
        glPolygonMode(GL_BACK, GL_POINT);
        glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
        glLineWidth(1.0f);
    }
    glRotatef(m_rotation * 360.0f, 0.0f, 1.0f, 0.0f);
    for (int j = 0; j < count; ++j)
        m_subs.m_items[j]->drawImmediate();
}

void Vani::read(iStream& in)
{
    clear();
    m_center.x = in.readFloat();
    m_center.y = in.readFloat();
    m_center.z = in.readFloat();
    m_rotation = in.readFloat();
    m_min.x = in.readFloat();
    m_min.y = in.readFloat();
    m_min.z = in.readFloat();
    m_max.x = in.readFloat();
    m_max.y = in.readFloat();
    m_max.z = in.readFloat();
    m_nSubItems = in.readInt();
    m_nSteps = in.readInt();
    m_unk74 = in.readInt();
    for (int i = 0; i < m_nSubItems; ++i)
    {
        VaniSubStruct* sub = new VaniSubStruct(m_nSteps);
        static_cast<Streamable*>(sub)->read(in, 0);
        m_subs.add(sub);
    }
}

bool Vani::loadTexture(const wchar_t* dir)
{
    if (!dir)
        return false;
    String path;
    path.append(dir, (unsigned)-1);
    for (int i = 0; i < m_subs.size(); ++i)
        m_subs.get(i)->loadTexture(path);
    return true;
}
