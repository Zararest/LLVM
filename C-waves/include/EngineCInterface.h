typedef struct WaveSource {
  size_t W;
  size_t A;

  size_t X;
  size_t Y;

  // initial parameters of the source
  size_t T0;
  size_t Phi0;

  // the end of source
  size_t AlreadyDoesntExist;
  size_t TEnd;
} WaveSource;

typedef struct WaveSourcesList {
  size_t Count;
  size_t MaxA;
  WaveSource *Arr;
} WaveSourcesList;

typedef struct SimConfig {
  double dT;
  void *WindowHandle;
  void *RenderHandle;
  size_t Height;
  size_t Width;
  WaveSourcesList Src;
} SimConfig;

void simPrologue(SimConfig *Config);
void configureFrame(SimConfig *Config);
void simEpilogue(SimConfig *Config);