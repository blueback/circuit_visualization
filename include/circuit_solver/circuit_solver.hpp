#include "raylib.h"
#include "standard_defs/standard_includes.hpp"
#include <vector>

class CircuitSolver {
private:
  static constexpr float SCREEN_WIDTH = 1920;
  static constexpr float SCREEN_HEIGHT = 1080;
  static constexpr float SCREEN_FPS = 60;
  static constexpr int INPUT_DEGREE = 15;
  static constexpr float NODE_RADIUS = 30.0f;

  void inputLayer(std::vector<Vector2> &input_positions);

  void firstAdderLayer(std::vector<Vector2> &first_adder_layer_positions);

public:
  void solve(void);
};
