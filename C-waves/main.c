#include "include/Driver.h"

#include <SDL.h>
#include <assert.h>

void processSim(size_t NumOfFrames, size_t Delay, SimConfig Config) {
  void *Handle = initWindow(Config.Height, Config.Width);
  Config.RenderHandle =
      SDL_CreateRenderer(Handle, -1, SDL_RENDERER_ACCELERATED);
  SDL_SetRenderDrawColor(Config.RenderHandle, 0, 0, 0, 0);
  SDL_RenderClear(Config.RenderHandle);

  Config.WindowHandle = Handle;
  SDL_Event Event;
  int Quit = 0;

  simPrologue(&Config);
  for (size_t i = 0; i < NumOfFrames; ++i) {
    updateWindow(&Config);
    SDL_Delay(Delay);
    printf("Frame: %li\n", i);
  }
  simEpilogue(&Config);

  while (1 && !Quit) {
    if (SDL_PollEvent(&Event) && Event.type == SDL_QUIT)
      break;
  }

  destroyWindow(Handle);
}



int main(int Argc, char **Argv) {
  SimConfig Config = {/*dT*/ 3,
                      /*WindowHandle*/ NULL,
                      /*RenderHandle*/ NULL,
                      /*Height*/ 800,
                      /*Width*/ 800};
  Argc--;
  Argv++;
  size_t NumOfFrames = 10;
  while (Argc > 0) {
    char *Option = Argv[0];
    Argv++;
    Argc--;
    if (strcmp(Option, "--frames-count") == 0) {
      assert(Argc > 0 && "Too few arguments for frames");
      NumOfFrames = strtol(Argv[0], NULL, 10);
      Argv++;
      Argc--;
      break;
    }

    if (strcmp(Option, "--width") == 0) {
      assert(Argc > 0 && "Too few arguments for width");
      Config.Width = strtol(Argv[0], NULL, 10);
      Argv++;
      Argc--;
      break;
    }

    if (strcmp(Option, "--height") == 0) {
      assert(Argc > 0 && "Too few arguments for height");
      Config.Height = strtol(Argv[0], NULL, 10);
      Argv++;
      Argc--;
      break;
    }

    if (strcmp(Option, "--dT") == 0) {
      assert(Argc > 0 && "Too few arguments for time");
      Config.dT = strtol(Argv[0], NULL, 10);
      Argv++;
      Argc--;
      break;
    }

    assert(0 && "Unknown option");
  }

  processSim(NumOfFrames, /*Delay*/ 0, Config);
}