#pragma once
#include "ptrArray.h"
#include "Scene.h"

class Scene;

class Scenes : public ptrArray<Scene*> {
public:
    Scenes() : ptrArray<Scene*>(256) {}
};
static_assert(sizeof(Scenes) == 0x18, "Scenes size");
