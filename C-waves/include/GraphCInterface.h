#include <stddef.h>
#include <stdint.h>

typedef struct Color {
  uint8_t R;
  uint8_t G;
  uint8_t B;
} Color;

void setPixel(size_t X, size_t Y, Color RGB, void *WindowHandle);

// Mult * cos(Arg)
long long multipliedCos(size_t Mult, long long Arg);

// Mult * sqrt(Arg)
long long multipliedSqrt(size_t Mult, size_t Arg);
long long sqrtli(size_t Arg);