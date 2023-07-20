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

void DrawShadedTriangle(SDL_Renderer* renderer, point p0, point p1, point p2, int color[]) {
  if (p1.y < p0.y) { Swap(&p1, &p0); }
  if (p2.y < p0.y) { Swap(&p2, &p0); }
  if (p2.y < p1.y) { Swap(&p2, &p1); }

  std::vector<float> x01 = Interpolate((float)p0.y, (float)p0.x, (float)p1.y, (float)p1.x);
  std::vector<float> h01 = Interpolate((float)p0.y, (float)p0.h, (float)p1.y, (float)p1.h);

  std::vector<float> x12 = Interpolate((float)p1.y, (float)p1.x, (float)p2.y, (float)p2.x);
  std::vector<float> h12 = Interpolate((float)p1.y, (float)p1.h, (float)p2.y, (float)p2.h);

  std::vector<float> x02 = Interpolate((float)p0.y, (float)p0.x, (float)p2.y, (float)p2.x);
  std::vector<float> h02 = Interpolate((float)p0.y, (float)p0.h, (float)p2.y, (float)p2.h);

  x01.pop_back();
  std::vector<float>& x012 = x01;
  x012.insert(x012.end(), x12.begin(), x12.end());

  h01.pop_back();
  std::vector<float>& h012 = h01;
  h012.insert(h012.end(), h12.begin(), h12.end());

  std::vector<float> x_left;
  std::vector<float> x_right;
  std::vector<float> h_left;
  std::vector<float> h_right;

  int m = floor(x012.size() / 2);
  if (x02[m] < x012[m]) {
    x_left = x02;
    h_left = h02;

    x_right = x012;
    h_right = h012;
  } else {
    x_left = x012;
    h_left = h012;

    x_right = x02;
    h_right = h02;
  }

  std::vector<float> h_segment;
  //int shaded_color[3];

  float x_l;
  float x_r;
  for (int y = p0.y; y < p2.y; y++) {
    x_l = x_left[y - p0.y];
    x_r = x_right[y - p0.y];
    h_segment = Interpolate(x_l, h_left[y - p0.y], x_r, h_right[y - p0.y]);
    for (int x = (int)x_l; x < (int)x_r; x++) {
      // TODO: make a utility function to abstract this, too many columns !!!
      int shaded_color[3] = {(int)(color[0] * h_segment[x - x_l]), (int)(color[1] * h_segment[x - x_l]), (int)(color[2] * h_segment[x - x_l])};
      DrawPoint(renderer, x, y, shaded_color);
    }
  }
}
