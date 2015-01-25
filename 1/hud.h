#ifndef _HUD_H
#define _HUD_H

#include "main.h"
//#include "glut.h"
#include <sstream>


void PerspectiveMode();
void OrthoMode(int left, int top, int right, int bottom);
//void drawText(float x, float y, float r, float g, float b, string s);
void drawGameOver();
void drawHud();
void drawClear();
#endif