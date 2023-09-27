typedef struct EngineConfig {
  double dT;
  void *WindowHandle;
} EngineConfig;  

void configureFrame(EngineConfig Config);