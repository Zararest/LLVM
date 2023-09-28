#include "GraphCInterface.h"
#include "EngineCInterface.h"



// main
void configureFrame(SimConfig Config) {
  Color White = {255, 255, 255};
  Color Black = {0, 0, 0};
  for (size_t X = 0; X < Config.Width; ++X)
    for (size_t Y = 0; Y < Config.Height; ++Y)
      if (X % 4 == 0) {
        setPixel(X, Y, White, Config.WindowHandle);
      } else {
        setPixel(X, Y, Black, Config.WindowHandle);
      }
}