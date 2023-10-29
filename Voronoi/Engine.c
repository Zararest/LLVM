#include "Graphics.h"

#define NULL 0

// Реализации диаграма Вороного с константным количеством точек

#define DOTS_NUM 10
#define GROWTH_SPEED 5
#define MAX_R SIM_X_SIZE 

#define SEED 1
#define FRAMES_NUM 1

const struct RGB BackGround = {10, 10, 10};


struct Dot {
  // Положение центра
  uint64_t X;
  uint64_t Y;
  // Увеличение радиуса за один фрейм
  uint64_t GrowthSpeed;
  // 1 - увеличивается, -1 - уменьшается
  int GrowthDirection;
  // Текущий радиус
  uint64_t R;
  // Цвет соответствующей области
  struct RGB Colour;
};

uint64_t xorshift() {
  static uint64_t Seed = SEED;
  Seed ^= Seed << 13;
  Seed ^= Seed >> 7;
  Seed ^= Seed << 17;
  return Seed; 
}

void dumpDots(const struct Dot *ArrayOfDots) {
  #ifdef DEBUG
  for (uint64_t i = 0; i < DOTS_NUM; ++i) {
    const struct Dot *CurDot = ArrayOfDots + i;
    printf("\nOn dot:\n");
    printf("\tX,Y = %lu %lu\n", CurDot->X, CurDot->Y);
    printf("\tR = %lu\n", CurDot->R);
    printf("\tSpeed = %lu\n", CurDot->GrowthSpeed);
    printf("\tDirection = %i\n", CurDot->GrowthDirection);
    printf("\tR,G,B = %u, %u, %u\n", CurDot->Colour.R, 
                                     CurDot->Colour.G,
                                     CurDot->Colour.B);
  }
  #endif
}

void changePosition(struct Dot *CurDot) {
  CurDot->R = 0;
  CurDot->GrowthSpeed = GROWTH_SPEED;
  CurDot->GrowthDirection = 1;
  CurDot->X = xorshift() % SIM_X_SIZE;
  CurDot->Y = xorshift() % SIM_Y_SIZE;
  CurDot->Colour.R = xorshift();
  CurDot->Colour.G = xorshift();
  CurDot->Colour.B = xorshift();
}

void updateDot(struct Dot *CurDot) { 
  if (CurDot->GrowthDirection == -1 && CurDot->R <= CurDot->GrowthSpeed) {
    changePosition(CurDot);
    return;
  }

  if (CurDot->R > MAX_R)
    CurDot->GrowthDirection = -1;
  CurDot->R += CurDot->GrowthSpeed * CurDot->GrowthDirection;
}

void changeState(struct Dot *ArrayOfDots) {
  for (int i = 0; i < DOTS_NUM; ++i)
    updateDot(&ArrayOfDots[i]);
}

uint64_t distance(uint64_t X1, uint64_t Y1, uint64_t X2, uint64_t Y2) {
  uint64_t dX = X2 > X1 ? (X2 - X1) : (X1 - X2);
  uint64_t dY = Y2 > Y1 ? (Y2 - Y1) : (Y1 - Y2);
  return dX * dX + dY * dY;
}

const struct Dot *getNearestDot(uint64_t X, uint64_t Y,
                                const struct Dot *ArrayOfDots) {
  const struct Dot *NearestDot = NULL;
  uint64_t MinDist = 0xBEBE;
  for (uint64_t i = 0; i < DOTS_NUM; ++i) {
    const struct Dot *CurDot = &ArrayOfDots[i];
    uint64_t Dist = distance(X, Y, CurDot->X, CurDot->Y);
    // Poison
    if (Dist < CurDot->R * CurDot->R && Dist < MinDist) {
      MinDist = Dist;
      NearestDot = CurDot;
    }
  }
  return NearestDot;
}

void drawFrame(const struct Dot *ArrayOfDots) {
  for (uint64_t X = 0; X < SIM_X_SIZE; ++X)
    for (uint64_t Y = 0; Y < SIM_Y_SIZE; ++Y) {
      const struct Dot *NearestDot = getNearestDot(X, Y, ArrayOfDots);
      if (NearestDot == NULL) {
        simPutPixel(X, Y, BackGround);
        continue;
      }
      simPutPixel(X, Y, NearestDot->Colour);
    } 
}

void initDots(struct Dot *ArrayOfDots) {
  for (int i = 0; i < DOTS_NUM; ++i)
    changePosition(&ArrayOfDots[i]);
}

void app() {
  struct Dot ArrayOfDots[DOTS_NUM];
  initDots(ArrayOfDots);
  for (uint64_t i = 0; i < FRAMES_NUM; ++i) {
    changeState(ArrayOfDots);
    drawFrame(ArrayOfDots);
    simFlush();
  }
}
