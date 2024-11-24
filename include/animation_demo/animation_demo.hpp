#ifndef __ANIMATION_DEMO_HPP__
#define __ANIMATION_DEMO_HPP__

#include "standard_defs/standard_defs.hpp"

class CircuitAnimationDemo {
private:
  static constexpr float SCREEN_WIDTH = 1920;
  static constexpr float SCREEN_HEIGHT = 1080;
  static constexpr Vector2 SCREEN_RESOLUTION = {.x = SCREEN_WIDTH,
                                                .y = SCREEN_HEIGHT};
  static constexpr float SCREEN_FPS = 60;
  static constexpr int INPUT_DEGREE = 7;
  static constexpr float NODE_RADIUS = 30.0f;

  static constexpr Rectangle SCREEN_RECT = {
      .x = 0, .y = 0, .width = SCREEN_WIDTH, .height = SCREEN_HEIGHT};

  Vector2 input_position;
  std::vector<Vector2> constant_layer_positions;
  std::vector<Vector2> adder_layer_positions;
  std::vector<Vector2> multiplier_layer_positions;

  void constantLayer(void);

  void adderLayer(void);

  void multiplierLayers(void);

  void drawCircuit(void);

public:
  void solve(void);
};

#endif // __ANIMATION_DEMO_HPP__
