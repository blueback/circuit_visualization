#include "circuit_solver/circuit_solver_self_test.hpp"
#include "circuit_solver/circuit_solver.hpp"

void CircuitSolverSelfTest::selfTest(void) {
  CircuitSolver circuit_solver;
#if 1
  circuit_solver.solve();
#else
  circuit_solver.render_video();
#endif
}
