#define SIM_X_SIZE 64
#define SIM_Y_SIZE 64

#include <stdint.h>

namespace lib {

struct RGB {
  uint8_t R;
  uint8_t G;
  uint8_t B;
};

void simFlush();
void simPutPixel(int x, int y, struct RGB rgb);
int simRand();

void simInit();
void simExit();

} // namespace lib