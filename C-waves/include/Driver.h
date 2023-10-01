#include <stddef.h>

#include "EngineCInterface.h"

void *initWindow(size_t Height, size_t Width);
void updateWindow(SimConfig *Config);
void destroyWindow(void *WindowHandle);
