#include "circuit_solver/circuit_solver_self_test.hpp"
#include "raylib_probe/raylib_probe_self_test.hpp"
#include "animation_demo/animation_demo_self_test.hpp"

int main() {
  CircuitSolverSelfTest circuit_solver_self_test;
  circuit_solver_self_test.selfTest();

  //RaylibProbeSelfTest raylib_probe_self_test;
  //raylib_probe_self_test.selfTest();

  //CircuitAnimationDemoSelfTest circuit_animation_demo_self_test;
  //circuit_animation_demo_self_test.selfTest();

  return 0;
}
