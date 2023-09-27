#include <SDL.h>
#include <SDL_image.h>
#include <SDL_timer.h>
#include <assert.h>

#include "../include/Driver.h"
#include "../include/GraphCInterface.h"

void *initWindow(size_t Height, size_t Width) {
  SDL_Window* Window = SDL_CreateWindow("Wave", 
                                        SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED, 
                                        Width, Height, 
                                        SDL_WINDOW_SHOWN);
  assert(Window);
  return Window;
}

void updateWindow(EngineConfig Config) {
  configureFrame(Config);
  SDL_UpdateWindowSurface(Config.WindowHandle);
}

void destroyWindow(void *WindowHandle) {
  SDL_DestroyWindow(WindowHandle);
  SDL_Quit();
}