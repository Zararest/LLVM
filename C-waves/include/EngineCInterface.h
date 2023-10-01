typedef struct SimConfig {
  double dT;
  void *WindowHandle;
  void *RenderHandle;
  size_t Height;
  size_t Width;
} SimConfig;

void configureFrame(SimConfig Config);