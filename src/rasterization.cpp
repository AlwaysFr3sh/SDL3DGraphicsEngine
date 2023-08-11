// rasterization.cpp
#include <vector>
#include <SDL.h>
#include "../headers/point.h"

void Swap(point *a, point *b) {
  point temp = *a;
  *a = *b;
  *b = temp;
}

// Converts X coordinate from system where 0 is at the center of the screen to the native system where 0 is the top left corner
int ConvertXCoordinate(int x) {
  return 500 + x;
}

// Converts Y coordinate from system where 0 is at the center of the screen to the native system where 0 is the top left corner
int ConvertYCoordinate(int y) {
  return 500 - y;
}

// Wrapper around SDL_RenderDrawPoint() 
void DrawPoint(SDL_Renderer* renderer, int x, int y, int color[]) {
  // Convert from center zero coordinates to corner coordinates
  x = ConvertXCoordinate(x);
  y = ConvertYCoordinate(y);
  // Draw
  SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], 255);
  SDL_RenderDrawPoint(renderer, x, y);
}

// Wrapper around SDL_RenderClear with color!
void FillScreen(SDL_Renderer* renderer, int color[]) {
  SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], 255);
  SDL_RenderClear(renderer);
}

// interpolate line between two points
std::vector<float> Interpolate(float i0, float d0, float i1, float d1) {
  if (i0 == i1) {
    return (std::vector<float>){ d0 };
  }
  std::vector<float> values;
  float a = (d1 - d0) / (i1 - i0);
  float d = d0;
  //for (int i1; ++i) {
  for (int i = (int)i0; i<(int)i1; i++) {
    values.push_back(d);
    d = d + a;
  }

  return values;
}

void DrawLine(SDL_Renderer* renderer, point p0, point p1, int color[]) {
   if (abs(p1.x - p0.x) > abs(p1.y - p0.y)) {
    if (p0.x > p1.x) {
      Swap(&p0, &p1);
    }
    std::vector<float> ys = Interpolate(p0.x, p0.y, p1.x, p1.y);
    for (int x=p0.x; x<p1.x; ++x) {
      DrawPoint(renderer, (int)x, (int)ys[x - p0.x], color);
    }
  } else {
    if (p0.y > p1.y) {
      Swap(&p0, &p1);
    }
    std::vector<float> xs = Interpolate(p0.y, p0.x, p1.y, p1.x);
    for (int y=p0.y; y<p1.y; ++y) {
      DrawPoint(renderer, (int)xs[y - p0.y], (int)y, color);
    }
  }
}

void DrawWireframeTriangle(SDL_Renderer* renderer, point p0, point p1, point p2, int color[]) {
  DrawLine(renderer, p0, p1, color);
  DrawLine(renderer, p1, p2, color);
  DrawLine(renderer, p0, p2, color);
}

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

  //std::cout << "x01: " << x01.size() << " x12: " << x12.size() << " x02: " << x02.size() << std::endl;
  //std::cout << "x_left: " << x_left.size() << " x_right: " << x_right.size() << std::endl;

  std::vector<float> z_segment;
  float x_l;
  float x_r;
  float z;

  // TODO: I feel like this shouldn't be necessary
  int min_length = std::min(x_left.size(), x_right.size());

  for (int y = p[0].y; y < (min_length + p[0].y); y++) {
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