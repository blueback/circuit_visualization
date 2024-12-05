#include "standard_defs/standard_defs.hpp"

class RayPlug {
private:
  static const char *STATIC_LIB = "librayplug.so";

public:
  void eventLoopUpdate(void);
};
