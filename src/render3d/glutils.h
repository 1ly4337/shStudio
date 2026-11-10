#pragma once
#include <windows.h>
#include <GL/gl.h>

void glClearColorRef(COLORREF color);
void setupDefaultLight();
void glRotateXYZ(float angleX, float angleY, float angleZ);
bool createTextureFromBitmap(HBITMAP bitmap, GLuint* texture);
void drawCube(float size);
