#include <iostream>
#include <SDL2/SDL.h>
#include <vector>

// Swap two integers
void Swap(int *a, int *b) {
  int temp = *a;
  *a = *b;
  *b = temp;
}

void SwapFloat(double *a, double *b) {
  double temp = *a;
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
std::vector<int> Interpolate(int i0, int d0, int i1, int d1) {
  if (i0 == i1) {
    return (std::vector<int>){d0};
  }
  std::vector<int> values;
  float a = (float)(d1 - d0) / (float)(i1 - i0);
  float d = (float)d0;
  for (int i=i0; i<i1; ++i) {
    values.push_back((int)d);
    d = d + a;
  }

  return values;
}

// interpolate line between two points
std::vector<double> InterpolateFloat(int i0, double d0, int i1, double d1) {
  if (i0 == i1) {
    return (std::vector<double>){d0};
  }
  std::vector<double> values;
  double a = (double)(d1 - d0) / (double)(i1 - i0);
  double d = (double)d0;
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
    std::vector<int> ys = Interpolate(x0, y0, x1, y1);
    for (int x=x0; x<x1; ++x) {
      DrawPoint(renderer, (int)x, (int)ys[x - x0], color);
    }
  } else {
		//std::cout << "Line x0: " << x0 << "y0: " << y0 << "x1: " << x1 << "y1: " << y1 << std::endl;
    if (y0 > y1) {
      Swap(&x0, &x1);
      Swap(&y0, &y1);
    }
    std::vector<int> xs = Interpolate(y0, x0, y1, x1);
    for (int y=y0; y<y1; ++y) {
      DrawPoint(renderer, (int)xs[y - y0], (int)y, color);
    }
  }
}

// Draw wireframe triangle (untested lol)
void DrawTriangle(SDL_Renderer* renderer, int x0, int y0, int x1, int y1, int x2, int y2, int color[]) {
  DrawLine(renderer, x0, y0, x1, y1, color);
  DrawLine(renderer, x1, y1, x2, y2, color);
  DrawLine(renderer, x2, y2, x0, y0, color);
}

// Draw solid color triangle
// TODO: figure out why this function hangs the program
void FillTriangle(SDL_Renderer* renderer, int x0, int y0, int x1, int y1, int x2, int y2, int color[]) {

  if (y1 < y0) { Swap(&x0, &x1); Swap(&y0, &y1); }
  if (y2 < y0) { Swap(&x0, &x2); Swap(&y0, &y2); }
  if (y2 < y1) { Swap(&x1, &x2); Swap(&y1, &y2); }

  std::vector<int> x01 = Interpolate(y0, x0, y1, x1);
  std::vector<int> x12 = Interpolate(y1, x1, y2, x2);
  std::vector<int> x02 = Interpolate(y0, x0, y2, x2);

  x01.pop_back();
  std::vector<int>& x012 = x01;
  x012.insert(x012.end(), x12.begin(), x12.end());

  std::vector<int> x_left;
  std::vector<int> x_right;
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

  std::cout << "finished" << std::endl;
}

void DrawShadedTriangle(SDL_Renderer* renderer, int x0, int y0, double h0, int x1, int y1, double h1, int x2, int y2, double h2, int color[]) {
  if (y1 < y0) { Swap(&x0, &x1); Swap(&y0, &y1); SwapFloat(&h0, &h1); }
  if (y2 < y0) { Swap(&x0, &x2); Swap(&y0, &y2); SwapFloat(&h0, &h2); }
  if (y2 < y1) { Swap(&x1, &x2); Swap(&y1, &y2); SwapFloat(&h1, &h2); }

  std::vector<int> x01 = Interpolate(y0, x0, y1, x1);
  std::vector<double> h01 = InterpolateFloat(y0, h0, y1, h1);

  std::vector<int> x12 = Interpolate(y1, x1, y2, x2);
  std::vector<double> h12 = InterpolateFloat(y1, h1, y1, h2);

  std::vector<int> x02 = Interpolate(y0, x0, y2, x2);
  std::vector<double> h02 = InterpolateFloat(y0, h0, y2, h2);

  x01.pop_back();
  std::vector<int>& x012 = x01;
  x012.insert(x012.end(), x12.begin(), x12.end());

  h01.pop_back();
  std::vector<double>& h012 = h01;
  h012.insert(h012.end(), h12.begin(), h12.end());

  std::vector<int> x_left;
  std::vector<int> x_right;
  std::vector<double> h_left;
  std::vector<double> h_right;

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

  for (int y=y0; y<y2; y++) {
    int x_l = x_left[y - y0];
    int x_r = x_right[y - y0];
    std::vector<double> h_segment = InterpolateFloat(x_l, h_left[y - y0], x_r, h_right[y - y0]);

    for (int x = x_l; x<x_r; x++) {
      //int shaded_color[3] = { (int)(color[0] * h_segment[x - x_l]), (int)(color[1] * h_segment[x - x_l]), (int)(color[2] * h_segment[x - x_l]) };
      int shaded_color[3] = { (int)round((color[0] * h_segment[x - x_l])), (int)round((color[1] * h_segment[x - x_l])), (int)round((color[2] * h_segment[x - x_l])) };
      DrawPoint(renderer, x, y, shaded_color);
    }
  }
}


// Main program loop TODO: do framerate stuff 
void ProgramLoop(SDL_Renderer* renderer) {
  int black[3] = {0, 0, 0};
  int white[3] = {255, 255, 255};
  int red[3] = {255, 0, 0};

  FillScreen(renderer, black);

  // This triangle, is broken, TODO: fix it maybe. 
  //DrawTriangle(renderer, 0, 0, 100, -200, 300, 400, red);

  //FillTriangle(renderer, 0, 0, 100, -200, 300, 400, red);
  //FillTriangle(renderer, -300, 300, -100, 300, -200, 200, red);
  //FillTriangle(renderer, -300, -300, -100, -351, -200, -200, red);
  //DrawShadedTriangle(renderer, 0, 0, 0.0, 100, -200, 0.5, 300, 400, 1.0, red);

  // Debug triangles
  DrawShadedTriangle(renderer, -200, -250, 0.3, 200, 50, 0.1, 20, 250, 1.0, red);

  FillTriangle(renderer, -450, 450, -450, 203, -356, 400, red);

  DrawTriangle(renderer, 300, -400, 400, -400, 350, -300, red);
  
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
