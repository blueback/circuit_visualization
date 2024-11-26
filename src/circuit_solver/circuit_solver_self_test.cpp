#include "circuit_solver/circuit_solver_self_test.hpp"
#include "circuit_solver/circuit_solver.hpp"

void CircuitSolverSelfTest::selfTest(void) {
  CircuitSolver circuit_solver;
  //circuit_solver.solve();
  circuit_solver.render_video();
}
