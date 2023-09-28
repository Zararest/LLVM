#include <stdint.h>
#include <stddef.h>

typedef struct Color {
  uint8_t R;
  uint8_t G;
  uint8_t B;
} Color;

void setPixel(size_t X, size_t Y, 
              Color RGB, 
              void *WindowHandle);