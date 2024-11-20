#include "circuit_solver/circuit_solver.hpp"
#include "raylib.h"

void CircuitSolver::solve() {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "circuit visualization");
  SetTargetFPS(SCREEN_FPS);

  while (!WindowShouldClose()) {
    BeginDrawing();
    {
      ClearBackground(BLACK);
      {
        DrawCircle(SCREEN_WIDTH / 3, SCREEN_HEIGHT / 3, 30.0, GetColor(0xcd5c5c));
        DrawCircle(2 * SCREEN_WIDTH / 3, 2 * SCREEN_HEIGHT / 3, 30.0, BLUE);
        DrawCircle(2 * SCREEN_WIDTH / 3, SCREEN_HEIGHT / 3, 30.0, GREEN);
        DrawCircle(SCREEN_WIDTH / 3, 2 * SCREEN_HEIGHT / 3, 30.0, YELLOW);
      }
    }
    EndDrawing();
  }
}
