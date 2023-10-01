#include "GraphCInterface.h"
#include "EngineCInterface.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Симуляция сферической волны
// На экране будет изображен профиль сферчиеской волны:
//  E = E_0 / r cos(k * r - w * (t - t_0) + phi_0)
//  r = sqrt((x - x_0)^2 - (y - y_0)^2)
//  k = w / c

#define C 100
#define R_UNIT 10

// чтобы волны было нормально видно их надо умножить на константу
#define A_BOOST 1000

long long getAmplitudeFromSrc(size_t X, size_t Y, size_t GlobalT,
                              const WaveSource *Src) {
  assert(Src);
  size_t RSquare = R_UNIT * R_UNIT *
                   ((X - Src->X) * (X - Src->X) + (Y - Src->Y) * (Y - Src->Y));
  size_t TimeToTravelSquare = RSquare / (C * C);
  assert(GlobalT >= Src->T0);
  if (RSquare == 0)
    return Src->A;
  if (TimeToTravelSquare > (GlobalT - Src->T0) * (GlobalT - Src->T0))
    return 0;
  if (Src->AlreadyDoesntExist &&
      (GlobalT - Src->TEnd) * (GlobalT - Src->TEnd) > TimeToTravelSquare)
    return 0;

  long long Arg = 
    multipliedSqrt(Src->W, TimeToTravelSquare) - Src->W * (GlobalT);
  return multipliedCos(Src->A * A_BOOST / sqrtli(RSquare), Arg);
}

Color getPixelColorJ(size_t X, size_t Y, size_t GlobalT,
                     const WaveSourcesList SrcArr) {;
  assert(SrcArr.Count);
  // результирующая амплитуда
  long long ARes = 0;
  for (size_t i = 0; i < SrcArr.Count; ++i)
    ARes += getAmplitudeFromSrc(X, Y, GlobalT, &SrcArr.Arr[i]);
  size_t J = ARes * ARes;
  size_t JMax = (SrcArr.MaxA * SrcArr.Count) * (SrcArr.MaxA * SrcArr.Count);
  long long NormilizedJ = J * 256 / JMax;

  Color Res = {NormilizedJ, NormilizedJ, NormilizedJ};
  return Res;
}

Color getPixelColorA(size_t X, size_t Y, size_t GlobalT,
                     const WaveSourcesList SrcArr) {
  assert(SrcArr.Count);
  // результирующая амплитуда
  long long ARes = 0;
  for (size_t i = 0; i < SrcArr.Count; ++i)
    ARes += getAmplitudeFromSrc(X, Y, GlobalT, &SrcArr.Arr[i]);
  size_t AMax = SrcArr.MaxA * SrcArr.Count;
  long long NormilizedA = ARes * 127 / AMax;

  Color Res = {NormilizedA + 127, NormilizedA + 127, NormilizedA + 127};
  return Res;
}

void addSources(size_t Height, size_t Width, WaveSourcesList *Src) {
  Src->Count = 2;
  Src->Arr = (WaveSource *)calloc(Src->Count, sizeof(WaveSource));
  Src->Arr[0].W = 5;
  Src->Arr[0].A = 10000;
  Src->Arr[0].Y = Height / 2;
  Src->Arr[0].X = Width / 2 - 50;

  Src->Arr[1].W = 6;
  Src->Arr[1].A = 10000;
  Src->Arr[1].Y = Height / 2;
  Src->Arr[1].X = Width / 2 + 50;

  Src->MaxA = 0;
  for (size_t i = 0; i < Src->Count; ++i)
    if (Src->Arr[i].A > Src->MaxA)
      Src->MaxA = Src->Arr[i].A;
  assert(Src->MaxA);
}

void deleteSources(WaveSourcesList Src) { free(Src.Arr); }

void simPrologue(SimConfig *Config) {
  WaveSourcesList Src = {};
  addSources(Config->Height, Config->Width, &Src);
  Config->Src = Src;
}

// main
void configureFrame(SimConfig *Config) {
  static size_t GlobalT = 10;
  for (size_t X = 0; X < Config->Width; ++X)
    for (size_t Y = 0; Y < Config->Height; ++Y)
#ifdef DRAW_A
      setPixel(X, Y, getPixelColorA(X, Y, GlobalT, Config->Src), Config->WindowHandle);
#else
      setPixel(X, Y, getPixelColorJ(X, Y, GlobalT, Config->Src), Config->WindowHandle);
#endif
  GlobalT += Config->dT;
}

void simEpilogue(SimConfig *Config) {
  deleteSources(Config->Src);
}