#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include "point.h"

void Swap(point *a, point *b);
int ConvertXCoordinate(int x);
int ConvertYCoordinate(int y);
void DrawPoint(SDL_Renderer* renderer, int x, int y, int color[]);
void FillScreen(SDL_Renderer* renderer, int color[]);
std::vector<float> Interpolate(float i0, float d0, float i1, float d1);
void DrawLine(SDL_Renderer* renderer, point p0, point p1, int color[]);
void DrawWireframeTriangle(SDL_Renderer* renderer, point p0, point p1, point p2, int color[]);
void DrawShadedTriangle(SDL_Renderer* renderer, point p0, point p1, point p2, int color[]); 
