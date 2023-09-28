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

void updateWindow(SimConfig Config) {
  configureFrame(Config);
  SDL_RenderPresent(Config.RenderHandle);
  SDL_UpdateWindowSurface(Config.WindowHandle);
}

void destroyWindow(void *WindowHandle) {
  SDL_DestroyWindow(WindowHandle);
  SDL_Quit();
}

void setPixel(size_t X, size_t Y, 
              Color RGB, 
              void *WindowHandle) {
  SDL_Renderer *Renderer = SDL_GetRenderer(WindowHandle);
  SDL_SetRenderDrawColor(Renderer, RGB.R, RGB.G, RGB.B, 255);
  SDL_RenderDrawPoint(Renderer, X, Y);
}