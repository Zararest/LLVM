#define SIM_X_SIZE 512
#define SIM_Y_SIZE 512

#include <stdint.h>

struct RGB {
  uint8_t R;
  uint8_t G;
  uint8_t B;
};

void simFlush();
void simPutPixel(int x, int y, struct RGB rgb);
int simRand();