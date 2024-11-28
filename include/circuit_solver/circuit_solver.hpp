#ifndef __CIRCUIT_SOLVER_HPP__
#define __CIRCUIT_SOLVER_HPP__

#include "circuit_animator/circuit_animator.hpp"

class ExampleCircuit001 : public CircuitModel {
private:
  void createCircuit(void);

public:
  ExampleCircuit001(void) { createCircuit(); }
};

class ExampleCircuit002 : public CircuitModel {
private:
  void createCircuit(void);

public:
  ExampleCircuit002(void) { createCircuit(); }
};

class ExampleCircuit003 : public CircuitModel {
private:
  void createCircuit(void);

public:
  ExampleCircuit003(void) { createCircuit(); }
};

namespace IntegerFactorization {

class RegularAPCircuit : public CircuitModel {
private:
  void createCircuit(const uint32_t degree);

public:
  RegularAPCircuit(void) = delete;
  RegularAPCircuit(const uint32_t degree) { createCircuit(degree); }
};

class Opt01Circuit : public CircuitModel {
private:
  void createCircuit(const uint32_t degree);

public:
  Opt01Circuit(void) = delete;
  Opt01Circuit(const uint32_t degree) { createCircuit(degree); }
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

  std::vector<CircuitModel *> _circuits;

  std::vector<CircuitAnimator> _animators;
  size_t _current_animator;

  void addOneCircuitToAnimate(CircuitModel *circuit);

  void stackCircuitsToAnimate(void);

  void drawVideoBackground(const bool use_mp);

  inline bool drawCircuits(const float time);

public:
  CircuitSolver(void) : _current_animator(0) {}

  void solve(void);

  void render_video(void);
};

#endif // __CIRCUIT_SOLVER_HPP__
