#include <iostream>
#include <SDL2/SDL.h>
#include <vector>
#include "../headers/point.h"
#include "../headers/rasterization.h"


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
