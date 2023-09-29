#include "GraphCInterface.h"
#include "EngineCInterface.h"

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

// Симуляция сферической волны
// На экране будет изображен профиль сферчиеской волны:
//  E = E_0 / r cos(k * r - w * (t - r / c))
//  r = sqrt((x - x_0)^2 - (y - y_0)^2)
//  k = w / c

#define C 100

typedef struct WaveSource {
  size_t W;
  size_t A;

  size_t X;
  size_t Y;

// initial parameters of the source
  size_t T0;
  size_t Phi0;
} WaveSource;

typedef struct WaveSourcesList {
  size_t Count;
  size_t MaxA;
  WaveSource *Arr;
} WaveSourcesList;

double getAmplitudeFromSrc(size_t X, size_t Y, 
                           size_t GlobalT, const WaveSource *Src) {
  assert(Src);
  size_t RSquare = 10 * (X - Src->X) * (X - Src->X) + 10 * (Y - Src->Y) * (Y - Src->Y);
  size_t TimeToTravelSquare = RSquare / (C * C);
  assert(GlobalT >= Src->T0);
  if (RSquare == 0)
    return Src->A;
  if (TimeToTravelSquare > (GlobalT - Src->T0) * (GlobalT - Src->T0))
    return 0;
  
  double ReExp = cos(Src->W / C - Src->W * (GlobalT - sqrt(TimeToTravelSquare)));
  return ((double) (Src->A * 1000)) / sqrt(RSquare) * ReExp;
}

Color getPixelColor(size_t X, size_t Y, size_t GlobalT, const WaveSourcesList *SrcArr) {
  assert(SrcArr);
  // результирующая амплитуда
  double ARes = 0;
  for (size_t i = 0; i < SrcArr->Count; ++i)
    ARes += getAmplitudeFromSrc(X, Y, GlobalT, &SrcArr->Arr[i]);
  ARes = ARes * ARes;
  double NormilizedA = ((double)ARes) / ((double)(SrcArr->MaxA * SrcArr->Count)) / ((double)(SrcArr->MaxA * SrcArr->Count)); 
  
  Color Res = {NormilizedA * 256, NormilizedA * 256, NormilizedA * 256};
  return Res;
}

void addSources(size_t Height, size_t Width, WaveSourcesList *Src) {
  Src->Count = 2;
  Src->Arr = (WaveSource *)calloc(Src->Count, sizeof(WaveSource));
  Src->Arr[0].W = 10;
  Src->Arr[0].A = 10000000;
  Src->Arr[0].Y = 0;
  Src->Arr[0].X = 0;
  
  Src->Arr[1].W = 10;
  Src->Arr[1].A = 10000000;
  Src->Arr[1].Y = Height;
  Src->Arr[1].X = 0;

  Src->MaxA = 0;
  for (size_t i = 0; i < Src->Count; ++i)
    if (Src->Arr[i].A > Src->MaxA)
      Src->MaxA = Src->Arr[i].A;
  assert(Src->MaxA);
}

void deleteSources(WaveSourcesList *Src) {
  free(Src->Arr);
}

// main
void configureFrame(SimConfig Config) {
  WaveSourcesList Src = {};
  addSources(Config.Height, Config.Width, &Src);
  static size_t GlobalT = 10;
  for (size_t X = 0; X < Config.Width; ++X)
    for (size_t Y = 0; Y < Config.Height; ++Y)
      setPixel(X, Y, getPixelColor(X, Y, GlobalT, &Src), Config.WindowHandle);
  GlobalT += Config.dT;
}