#include "include/Driver.h"

void processSim(size_t Height, size_t Width, size_t NumOfFrames, 
                size_t Delay, EngineConfig Config) {
  void *Handle = initWindow(Height, Width);
  Config.WindowHandle = Handle;
  for (size_t i = 0; i < NumOfFrames; ++i) {
    updateWindow(Config);
    SDL_Delay(Delay);
  }
  destroyWindow(Handle);
}

int main(int Argc, char **Argv) {
  EngineConfig Config = {0.1, NULL};
  processSim(/*Height*/500, /*Width*/ 500, 
             /*NumOfFrames*/ 30, /*Delay*/ 100, 
             Config);
}