// main.cpp

#include <iostream>
#include <vector>
#include <cmath>
#include <SDL2/SDL.h>
#include "../headers/point.h"
#include "../headers/rasterization.h"
#include "../headers/render.h"

// ---------
// Clipping |
// ---------
/*
struct plane {
  vertex normal;
  int distance;
};

vertex ApproximateBoundingSphereCenter(mesh m) {
  vertex ret = {0, 0, 0};
  for (vertex v : m.vertices) {
    ret.x += v.x;
    ret.y += v.y;
    ret.z += v.z;
  }
  int size = m.vertices.size();
  ret.x = ret.x / size;
  ret.y = ret.y / size;
  ret.z = ret.z / size;
  return ret; 
}

float ApproximateBoundingSphereRadius(mesh m, vertex r) {
  float ret = 0;
  float distance;
  for (vertex v : m.vertices) {
    distance = abs(sqrt(pow(v.x - r.x, 2) + pow(v.y - r.y, 2) + pow(v.z - r.z, 2)));
    if (distance > ret)
      ret = distance;
  }
  // TODO: find out if return value should be absolute
  return ret;
}

// TODO: figure out how to ignore mesh when completely outside clipping planes
// Maybe No vertices counts as an ignore?
mesh Clip(std::vector<plane> planes, mesh m) {
  mesh ret;
  for (plane p : planes) {
    ret = ClipAgainstPlane(p, m);
  } 
  return ret;
}

mesh ClipAgainstPlane(plane p, mesh m) {
  vertex meshCenter = ApproximateBoundingSphereCenter(m);
  float d = SignedDistance(p, meshCenter);
  float r = ApproximateBoundingSphereRadius(m, meshCenter);

  if (d > r) {
    return m;
  } else if (d < -r) {
    //return NULL;
    mesh ignored;
    std::cout << ignored.vertices.size() <<  std::endl; // Confirm that this prints zero and doesn't crash
    return ignored;
  } else {
    // apparently according to stackoverflow this is a copy after complation
    mesh clipped = m; 
    clipped.triangles = ClipTriangles(m.triangles, m.vertices, p);
    return clipped;
  }
}

// TODO: do I pass mesh as a whole? or two lists separately?
std::vector<triangle> ClipTriangles(std::vector<triangle> triangles, std::vector<vertex> vertices, plane p) {
  std::vector<triangle> clipped = {};
  for (triangle t : triangles){
    triangle clipped_t = ClipTriangle(t, vertices, p);
    if (clipped_t.points[0] != NULL)
      clipped.push_back(clipped_t);
  }
  return clipped;
}

void SegmentPlaneIntersection(float distance, plane p) {

}

// TODO: make a helper function file and stick this in it
#include <stdlib.h>

int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

triangle ClipTriangle(triangle t, std::vector<vertex> vertices, plane p) {

  float d[3] = {SignedDistance(p, vertices[t.points[0]]), 
                SignedDistance(p, vertices[t.points[1]]), 
                SignedDistance(p, vertices[t.points[2]])};

  qsort(d, 3, sizeof(float), cmpfunc);

  // TODO: checking every value is pointless when they are sorted (if 0 is positive, all are etc)
  if (d[0] > 0 && d[1] > 0 && d[2] > 0) {
    return t;
  } else if (d[0] < 0 && d[1] < 0 && d[2] < 0) {
    triangle clipped = {NULL};
    return clipped;
  } else if (d[0] < 0 && d[1] < 0 && d[2] > 0) {
    
  } else {

  }

  
}

float SignedDistance(plane p, vertex v) {
  return (v.x * p.normal.x) + (v.y * p.normal.y) + (v.z + p.normal.z) + p.distance;
}
*/

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
  cube = Translate(cube, 10, 4, 50);

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
