#include "kWorld_Section.h"

namespace kWorld
{
    const wchar_t* g_sectionNames[12] = {
        L"Building",
        L"Shape",
        L"Tree",
        L"Grass",
        L"VAni",
        L"VAni(2)",
        L"Dungeon",
        L"MAni",
        L"Object",
        L"Musics (foreground)",
        L"Sounds (background)",
        L"Vertices",
    };

    const wchar_t* g_sectionCaptions[12] = {
        L"Buildings (Entity/Building)",
        L"Shapes (Entity/Shape)",
        L"Trees (Entity/Tree)",
        L"Grass (Entity/Grass)",
        L"VANI (Entity/Vani)",
        L"VANI(2) (Entity/Vani)",
        L"Dungeon (World/dungeon)",
        L"MANI (Entity/Mani)",
        L"Objects (Entity/Object)",
        L"Musics (Sound/Music)",
        L"Background Musics (Sound/Back)",
        L"Vertices",
    };

    wchar_t g_textBuffer[256];
}
