#include "standard_defs/standard_includes.hpp"

class RaylibProbe {
private:
  static constexpr float SCREEN_WIDTH = 900;
  static constexpr float SCREEN_HEIGHT = 900;
  static constexpr float DVD_SCALING = 0.125f;
  static constexpr float SCREEN_FPS = 60;
  static constexpr float DELTA_TIME = (1.0f / SCREEN_FPS);
  static constexpr float DVD_VELOCITY = 100.0f;

public:
  void runRaylibProbe(void);
};
