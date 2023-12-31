#define SIM_X_SIZE 256
#define SIM_Y_SIZE 256

#include <stdint.h>

namespace lib {

struct RGB {
  uint8_t R;
  uint8_t G;
  uint8_t B;
};

void simFlush();
void simPutPixel(long long x, long long y, struct RGB rgb);
int simRand();

void simInit();
void simExit();

} // namespace lib