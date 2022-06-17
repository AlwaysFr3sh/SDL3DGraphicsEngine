#include <iostream>
#include <SDL2/SDL.h>
#include <vector>

struct point {
  int x;
  int y;
  double h;
};

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

void DrawShadedTriangle(SDL_Renderer* renderer, point p0, point p1, point p2, int color[]) {
  if (p1.y < p0.y) { Swap(&p1, &p0); }
  if (p2.y < p0.y) { Swap(&p2, &p0); }
  if (p2.y < p1.y) { Swap(&p2, &p1); }
  // TODO: convert integer to float to pass to Interpolate()
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

// Main program loop TODO: do framerate stuff 
void ProgramLoop(SDL_Renderer* renderer) {
  int black[3] = {0, 0, 0};
  int white[3] = {255, 255, 255};
  int red[3] = {255, 0, 0};
  int green[3] {0, 255, 0};

  // For now we draw before loop
  FillScreen(renderer, white);

  point p0;
  p0.x = -200;
  p0.y = -250;
  p0.h = 0.3f;
  
  point p1;
  p1.x = 200;
  p1.y = 50;
  p1.h = 0.1;

  point p2;
  p2.x = 20;
  p2.y = 250;
  p2.h = 1.0;

  // Debug triangles
  DrawShadedTriangle(renderer, p0, p1, p2, green);


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
  SDL_Window* window = CreateWindowContext((const char*)"Graphics", 1000, 1000); 

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
