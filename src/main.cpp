// main.cpp

#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <SDL2/SDL.h>
#include "../headers/point.h"
#include "../headers/rasterization.h"
#include "../headers/render.h"


void DrawCircle(SDL_Renderer* renderer, int32_t centreX, int32_t centreY, int32_t radius)
{
int black[3] = {0, 0, 0};

const int32_t diameter = (radius * 2);

int32_t x = (radius - 1);
int32_t y = 0;
int32_t tx = 1;
int32_t ty = 1;
int32_t error = (tx - diameter);

while (x >= y)
{
// Each of the following renders an octant of the circle
DrawPoint(renderer, centreX + x, centreY - y, black);
DrawPoint(renderer, centreX + x, centreY + y, black);
DrawPoint(renderer, centreX - x, centreY - y, black);
DrawPoint(renderer, centreX - x, centreY + y, black);
DrawPoint(renderer, centreX + y, centreY - x, black);
DrawPoint(renderer, centreX + y, centreY + x, black);
DrawPoint(renderer, centreX - y, centreY - x, black);
DrawPoint(renderer, centreX - y, centreY + x, black);

  if (error <= 0)
  {
  	++y;
  	error += ty;
  	ty += 2;
  }

  if (error > 0)
  {
  	--x;
  	tx += 2;
  	error += (tx - diameter);
  }
}
}

// TODO: abstract all the dot products to a function

// Main program loop TODO: do framerate stuff 
// TODO: do loop xD
void Program(SDL_Renderer* renderer) {
  int black[3] = {0, 0, 0};
  int white[3] = {255, 255, 255};
  int red[3] = {255, 0, 0};
  int green[3] {0, 255, 0};

  FillScreen(renderer, white);

  mesh cube;

  cube.vertices = {
    {1, 1, 1},
    {-1, 1, 1},
    {-1, -1, 1},
    {1, -1, 1},
    {1, 1, -1},
    {-1, 1, -1},
    {-1, -1, -1},
    {1, -1, -1},
  };

  cube.triangles = {
    {0, 1, 2},
    {0, 2, 3},
    {4, 0, 3},
    {4, 3, 7},
    {5, 4, 7}, 
    {5, 7, 6},
    {1, 5, 6},
    {1, 6, 2},
    {4, 5, 1},
    {4, 1, 0},
    {2, 6, 7},
    {2, 7, 3},
  };

  // translation
  cube = Translate(cube, -5, 0, 10);

  // ripped of the js implementation of this stuff
  float s2 = (float)std::sqrt(2);

  std::vector<plane> planes = {
    plane({0, 0, 1}, -1),
    plane({s2, 0, s2}, 0),
    plane({-s2, 0, s2}, 0),
    plane({0, -s2, s2}, 0),
    plane({0, s2, s2}, 0)
  };

  // Projection
  projectedMesh projectedCube = ProjectMesh(cube);

  // Draw
  DebugDrawMesh(renderer, projectedCube, black);

  SDL_RenderPresent(renderer);

  while (true) {
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        // break program loop
        break;
      }
    }
  }
}

// Creates SDL Window context (???)
SDL_Window* CreateWindowContext(const char* title, int width, int height) {
  SDL_Window *window;                    // Declare a pointer
  window = SDL_CreateWindow(
      title,                             // window title
      SDL_WINDOWPOS_UNDEFINED,           // initial x position
      SDL_WINDOWPOS_UNDEFINED,           // initial y position
      width,                             // width, in pixels
      height,                            // height, in pixels
      SDL_WINDOW_OPENGL                  // flags - see below
  );
  return window;
}

// Entry Point
int main(int argc, char* argv[]) {
  if (SDL_Init(SDL_INIT_VIDEO)) {              // Initialize SDL2
    std::cout << "SDL_INIT has broken. SDL_ERROR: " << SDL_GetError() << std::endl;
  }
  // Create an application window with the following settings:
  SDL_Window* window = CreateWindowContext((const char*)"Graphics", WIDTH, HEIGHT); 
  // Check that the window was successfully created
  if (window == NULL) {
      // In the case that the window could not be made...
      printf("Could not create window: %s\n", SDL_GetError());
      return 1;
  }
  // init renderer
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == nullptr) {
    std::cout << "Could not create a renderer!" << SDL_GetError() << std::endl;
    return -1;
  }
  // The window is open: could enter program loop here (see SDL_PollEvent())
  // We are entering Program Loop here hehe :)
  Program(renderer); 
  // Destroy Renderer
  SDL_DestroyRenderer(renderer);
  // Close and destroy the window
  SDL_DestroyWindow(window);
  // Clean up
  SDL_Quit();
  return 0;
}
