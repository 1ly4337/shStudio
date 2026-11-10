#include "Textures.h"

Textures::Entry* Textures::clearEntry(Entry* e)
{
    if (e->value)
        glDeleteTextures(1, &e->value);
    e->key = 0;
    e->value = 0;
    return e;
}
