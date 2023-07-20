// main.cpp

#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <SDL.h>
#include "../headers/point.h"
#include "../headers/rasterization.h"
#include "../headers/render.h"

/*
TODO:

- Fix weird edge-depth bug that causes the behind color to sometimes bleed through
  - It's possible that this is because there are gaps in the triangles, to test this, try removing 
    the behind triangles manually and see if there is any 'white' that bleeds through.
  - If I am right about the triangle gaps, I'm not sure how to proceed. Instead of looping to the end of the smaller of
    x_left and x_right, I'll have to somehow find a way to ensure that the two are always the same length possibly?? 
    - To do this past me suggests the following:
      - Check the lengths of x01, x12 and x20 I expect them to not always be the same length
      - If my previous hypothesis is correct, the problem may be due to the int cast that happens in the Interpolate() function
      - Find out why (int)i1 does not always return the same result. 
      - If Interpolate() is not the problem, the problem is likely to have something to do with how we are stitching x01 and x12 together. 

- Refactor Triangle() function (remove duplicate code as much as possible)
- Put Triangle() function back inside the rasterization file (replace existing triangle function)
- Lighting 
- File Reading
- Back face culling
- Rotation
*/

void Triangle(SDL_Renderer* renderer, std::vector<std::vector<float>>& depthBuffer, point p[], vertex v[], int color[]) {
  
  if (p[1].y < p[0].y) { Swap(&p[1], &p[0]), std::swap(v[1], v[0]); }
  if (p[2].y < p[0].y) { Swap(&p[2], &p[0]), std::swap(v[2], v[0]); }
  if (p[2].y < p[1].y) { Swap(&p[2], &p[1]), std::swap(v[2], v[1]); }
 
  std::vector<float> x01 = Interpolate((float)p[0].y, (float)p[0].x, (float)p[1].y, (float)p[1].x);
  std::vector<float> z01 = Interpolate((float)p[0].y, 1.0 / (float)v[0].z, (float)p[1].y, 1.0 / (float)v[1].z);

  std::vector<float> x12 = Interpolate((float)p[1].y, (float)p[1].x, (float)p[2].y, (float)p[2].x);
  std::vector<float> z12 = Interpolate((float)p[1].y, 1.0 / (float)v[1].z, (float)p[2].y, 1.0 / (float)v[2].z);

  std::vector<float> x02 = Interpolate((float)p[0].y, (float)p[0].x, (float)p[2].y, (float)p[2].x);
  std::vector<float> z02 = Interpolate((float)p[0].y, 1.0 / (float)v[0].z, (float)p[2].y, 1.0 / (float)v[2].z);

  x01.pop_back();
  std::vector<float>& x012 = x01;
  x012.insert(x012.end(), x12.begin(), x12.end());

  z01.pop_back();
  std::vector<float>& z012 = z01;
  z012.insert(z012.end(), z12.begin(), z12.end());

  std::vector<float> x_left;
  std::vector<float> x_right;
  std::vector<float> z_left;
  std::vector<float> z_right;

  int m = floor(x012.size() / 2);
  if (x02[m] < x012[m]) {
    x_left = x02;
    z_left = z02;

    x_right = x012;
    z_right = z012;
  } else {
    x_left = x012;
    z_left = z012;

    x_right = x02;
    z_right = z02;
  }

  std::vector<float> z_segment;
  float x_l;
  float x_r;
  float z;

  int penis = std::min(x_left.size(), x_right.size());

  for (int y = p[0].y; y < (penis + p[0].y); y++) {
    x_l = x_left[y - p[0].y];
    x_r = x_right[y - p[0].y];

    z_segment = Interpolate(x_l, z_left[y - p[0].y], x_r, z_right[y - p[0].y]);

    for (int x = (int)x_l; x < (int)x_r; x++) {

      z = z_segment[(int)(x - x_l)];
      int adjusted_x = (WIDTH/2) + (x | 0);
      int adjusted_y = (HEIGHT/2) - (y | 0) - 1;

      if (z > depthBuffer[adjusted_x][adjusted_y]) {
        DrawPoint(renderer, x, y, color);
        depthBuffer[adjusted_x][adjusted_y] = z;
      } 
    }
  }
}

void DrawMesh(SDL_Renderer* renderer, projectedMesh m, int color[]) {

  std::vector<std::vector<float>> depthBuffer(WIDTH, std::vector<float>(HEIGHT, 0.0f));
  point points[3];
  vertex vertices[3];

  // color stuff, temporary to verify some stuff
  int colors[6][3] = {{255, 0, 0}, {0, 255, 0}, {0, 0, 255}, {255, 255, 0}, {0, 255, 255}, {255, 0, 255}};
  int color_index = 0;
  int index = 0;

  for ( triangle t : m.triangles ) {

    points[0] = m.points[t.points[0]], points[1] = m.points[t.points[1]], points[2] = m.points[t.points[2]];
    vertices[0] = m.vertices[t.points[0]], vertices[1] = m.vertices[t.points[1]], vertices[2] = m.vertices[t.points[2]];
    Triangle(renderer, depthBuffer, points, vertices, colors[color_index]);

    // Temporary color funk
    index++; 
    if (index % 2 == 0 && index != 0) {
      color_index++;
    }
    if (color_index > 5) {
      color_index = 0;
    }
    
  }
}

void Rasterizer(SDL_Renderer* renderer) {
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
    {1, 6, 2}, // debug this one
    {4, 5, 1},
    {4, 1, 0},
    {2, 6, 7},
    {2, 7, 3},
  };

  // translation
  cube = Translate(cube, 2, -2, 10);

  // Projection
  projectedMesh projectedCube = ProjectMesh(cube);

  // Draw
  DrawMesh(renderer, projectedCube, black);
  //DebugDrawMesh(renderer, projectedCube, black);

  SDL_RenderPresent(renderer);
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
  Rasterizer(renderer); 

  while (true) {
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        // break program loop
        break;
      }
    }
  }

  // Destroy Renderer
  SDL_DestroyRenderer(renderer);
  // Close and destroy the window
  SDL_DestroyWindow(window);
  // Clean up
  SDL_Quit();
  return 0;
}
