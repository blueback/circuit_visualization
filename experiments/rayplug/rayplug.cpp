#include "circuit_solver/rayplug/rayplug.hpp"
#include "dlfcn.h"
#include <dlfcn.h>

void RayPlug::eventLoopUpdate(void) {
  void *dll = dlopen(STATIC_LIB, RTLD_NOW);

  if (!dll) {
    char *dllLibPath = getenv("LD_LIBRARY_PATH");
    printf("LD_LIBRARY_PATH: %s\n", dllLibPath);
    printf("Not able to load %s\n", STATIC_LIB);
    assert(0);
    return;
  }
}
