#include "include/Driver.h"

#include <SDL.h>

void processSim(size_t NumOfFrames, 
                size_t Delay, SimConfig Config) {
  void *Handle = initWindow(Config.Height, Config.Width);
  Config.RenderHandle = SDL_CreateRenderer(Handle, -1, SDL_RENDERER_ACCELERATED);
  SDL_SetRenderDrawColor(Config.RenderHandle, 0, 0, 0, 0);
  SDL_RenderClear(Config.RenderHandle);

  Config.WindowHandle = Handle;
  for (size_t i = 0; i < NumOfFrames; ++i) {
    updateWindow(Config);
    SDL_Delay(Delay);
  }
  destroyWindow(Handle);
}

int main(int Argc, char **Argv) {
  SimConfig Config = {/*dT*/ 0.1, 
                      /*WindowHandle*/ NULL, 
                      /*RenderHandle*/ NULL,
                      /*Height*/ 500, 
                      /*Width*/  500};
  processSim(/*NumOfFrames*/ 30, /*Delay*/ 100, Config);
}