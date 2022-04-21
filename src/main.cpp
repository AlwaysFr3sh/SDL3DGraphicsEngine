#include <iostream>
#include <SDL2/SDL.h>
#include <vector>

// Swap two integers
void Swap(int *a, int *b) {
  int temp = *a;
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
std::vector<float> Interpolate(int i0, int d0, int i1, int d1) {
  if (i0 == i1) {
    return (std::vector<float>){(float)d0};
  }
  std::vector<float> values;
  float a = (float)(d1 - d0) / (float)(i1 - i0);
  float d = (float)d0;
  for (int i=i0; i<i1; ++i) {
    values.push_back(d);
    d = d + a;
  }
  return values;
}

// Draws Line
void DrawLine(SDL_Renderer* renderer, int x0, int y0, int x1, int y1, int color[]) {
  if (abs(x1 - x0) > abs(y1 - y0)) {
    if (x0 > x1) {
      Swap(&x0, &x1);
      Swap(&y0, &y1);
    }
    std::vector<float> ys = Interpolate(x0, y0, x1, y1);
    for (int x=x0; x<x1; ++x) {
      DrawPoint(renderer, x, ys[x - x0], color);
    }
  } else {
    if (y0 > y1) {
      Swap(&x0, &x1);
      Swap(&y0, &y1);
    }
    std::vector<float> xs = Interpolate(x0, y0, x1, y1);
    for (int y=y0; y<y1; ++y) {
      DrawPoint(renderer, xs[y - y0], y, color);
    }
  }
}

// Draw wireframe triangle (untested lol)
// TODO: figure out why this causes a segmentation fault
void DrawTriangle(SDL_Renderer* renderer, int x0, int y0, int x1, int y1, int x2, int y2, int color[]) {
  DrawLine(renderer, x0, y0, x1, y1, color);
  DrawLine(renderer, x1, y1, x2, y2, color);
  DrawLine(renderer, x2, y2, x0, y0, color);
}

// Draw solid color triangle
// TODO: figure out why this function hangs the program
void FillTriangle(SDL_Renderer* renderer, int x0, int y0, int x1, int y1, int x2, int y2, int color[]) {
  // Implement here! 
  if (y1 < y0) { Swap(&x0, &x1); Swap(&y0, &y1); }
  if (y2 < y0) { Swap(&x0, &x2); Swap(&y0, &y2); }
  if (y2 < y1) { Swap(&x1, &x2); Swap(&y1, &y2); }

  std::vector<float> x01 = Interpolate(y0, x0, y1, x1);
  std::vector<float> x12 = Interpolate(y1, x1, y2, x2);
  std::vector<float> x02 = Interpolate(y0, x0, y2, x2);

  x01.pop_back();
  std::vector<float> x012 = x01;
  x012.insert(x012.end(), x12.begin(), x12.end());

  std::vector<float> x_left;
  std::vector<float> x_right;
  int m = floor(x012.size() / 2);
  if (x02[m] < x012[m]) {
    x_left = x02;
    x_right = x012;
  } else {
    x_left = x012;
    x_right = x02;
  }

  for (int y=y0; y<y2; y++) {
    for (int x=x_left[y - y0]; x<x_right[y - y0]; x++) {
      DrawPoint(renderer, x, y, color);
    }
  }
}

// Main program loop TODO: do framerate stuff 
void ProgramLoop(SDL_Renderer* renderer) {
  int black[3] = {0, 0, 0};
  int white[3] = {255, 255, 255};
  int red[3] = {255, 0, 0};
  while (true) {
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        // break program loop
        break;
      }
    }
    FillScreen(renderer, black);
    //DrawLine(renderer, -200, -100, 240, 120, white);
    //DrawLine(renderer, -100, 0, 100, 0, white);
    //DrawLine(renderer, 0, -100, 0, -200, red);
    //FillTriangle(renderer, 0, 0, 100, -200, 300, 400, red);
    DrawTriangle(renderer, 0, 0, 100, -200, 300, 400, white);
    SDL_RenderPresent(renderer);
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
    std::cout << "SDL_INIT Fucked up bro... sorry for ur loss SDL_ERROR: " << SDL_GetError() << std::endl;
  }

  // Create an application window with the following settings:
  SDL_Window* window = CreateWindowContext((const char*)"cock and balls", 1000, 1000); 

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
  ProgramLoop(renderer); 

  // Destroy Renderer
  SDL_DestroyRenderer(renderer);

  // Close and destroy the window
  SDL_DestroyWindow(window);

  // Clean up
  SDL_Quit();

  return 0;
}
