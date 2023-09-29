#include "include/Driver.h"

#include <SDL.h>

void processSim(size_t NumOfFrames, 
                size_t Delay, SimConfig Config) {
  void *Handle = initWindow(Config.Height, Config.Width);
  Config.RenderHandle = SDL_CreateRenderer(Handle, -1, SDL_RENDERER_ACCELERATED);
  SDL_SetRenderDrawColor(Config.RenderHandle, 0, 0, 0, 0);
  SDL_RenderClear(Config.RenderHandle);

  Config.WindowHandle = Handle;
  int Quit = 0;
  for (size_t i = 0; i < NumOfFrames; ++i) {
    updateWindow(Config);
    SDL_Delay(Delay);
    printf("Frame: %li\n", i);
  }

  while (1 && !Quit) {
    SDL_Event Event;
    if (SDL_PollEvent(&Event) && Event.type == SDL_QUIT)
      break;
  }

  destroyWindow(Handle);
}

int main(int Argc, char **Argv) {
  SimConfig Config = {/*dT*/ 10, 
                      /*WindowHandle*/ NULL, 
                      /*RenderHandle*/ NULL,
                      /*Height*/ 1000, 
                      /*Width*/  1000};
  processSim(/*NumOfFrames*/ 100, /*Delay*/ 1, Config);
}