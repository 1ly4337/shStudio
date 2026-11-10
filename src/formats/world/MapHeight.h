#pragma once
#include "p04_common.h"
#include <GL/gl.h>
#include "ToggleNode.h"
#include "Struct3D.h"
#include "Strings.h"
#include "Textures.h"

class iStream;
class oStream;
class ListCtrl;
namespace kWorld { class Items; }

class MapHeight : public ToggleNode, public Struct3D
{
public:
    struct GridPoint
    {
        float x, y, z;
    };

    explicit MapHeight(kWorld::Items* owner);
    virtual ~MapHeight();

    void init(int mapSize);
    bool loadTextures();
    bool loadMiniMap();

    virtual void clear() override;
    virtual void computeBounds() override;
    virtual void fillProperties(ListCtrl* list) override;
    virtual void drawFaces() override;
    virtual GLenum drawTextured() override;

    virtual void read(iStream& in, DWORD fmt) override;
    virtual void write(oStream& out, DWORD fmt) const override;

public:
    int m_mapId;
    kWorld::Items* m_owner;
    int m_size;
    GridPoint* m_grid;
    BYTE* m_texIndex;
    HGDIOBJ* m_bitmaps;
    GLuint* m_tileTextures;
    Strings m_strings;
    Textures m_textures;
    bool m_followEye;
    int m_drawMode;
};
static_assert(sizeof(MapHeight) == 0x160, "MapHeight size");
