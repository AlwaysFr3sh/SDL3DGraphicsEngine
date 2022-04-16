#include <iostream>
#include <SDL2/SDL.h>

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

// Draws Line TODO: Implement
void DrawLine(SDL_Renderer* renderer, int x0, int y0, int x1, int y1) {

  return;

}

// Main program loop TODO: add loop
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
    DrawPoint(renderer, 100, 100, white); 
    DrawPoint(renderer, 100, 200, white);
    DrawPoint(renderer, 1, 1, red);
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
