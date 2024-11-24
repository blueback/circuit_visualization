#ifndef __CIRCUIT_SOLVER_HPP__
#define __CIRCUIT_SOLVER_HPP__

#include "circuit_animator/circuit_animator.hpp"

namespace IntegerFactorization {
class RegularAPCircuit : public CircuitModel {
public:
  void createCircuit(const uint32_t degree);
};
class Opt01Circuit : public CircuitModel {
public:
  void createCircuit(const uint32_t degree);
};
}; // namespace IntegerFactorization

class CircuitSolver {
private:
  static constexpr float SCREEN_WIDTH = 1920;
  static constexpr float SCREEN_HEIGHT = 1080;
  static constexpr Vector2 SCREEN_RESOLUTION = {.x = SCREEN_WIDTH,
                                                .y = SCREEN_HEIGHT};
  static constexpr float SCREEN_FPS = 60;
  static constexpr Rectangle SCREEN_RECT = {
      .x = 0, .y = 0, .width = SCREEN_WIDTH, .height = SCREEN_HEIGHT};

  void drawCircuit(CircuitAnimator &circuit_animator, const float time);

public:
  void solve(void);
};

#endif // __CIRCUIT_SOLVER_HPP__
