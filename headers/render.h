#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include "point.h"
#include "macros.h"

point ViewportToCanvas(float x, float y);
point project(vertex v);
projectedMesh ProjectMesh(mesh m);
mesh Translate(mesh m, int x, int y, int z);
// mesh transform()
void DebugDrawMesh(SDL_Renderer* renderer, projectedMesh, int color[]);
