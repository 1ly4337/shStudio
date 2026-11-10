#pragma once
#include <windows.h>
#include <GL/gl.h>
#include "ida_compat.h"
class iStream;
class oStream;
class String;

extern HINSTANCE hModule;
class StudioFrame;
extern StudioFrame* g_studioFrame;
void readSkillColumnHeaders(iStream& s);
void writeSkillColumnHeaders(oStream& s);
