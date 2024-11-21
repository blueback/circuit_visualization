#include "circuit_solver/circuit_solver.hpp"
#include "raylib.h"
#include "raymath.h"
#include <vector>

void CircuitSolver::inputLayer(std::vector<Vector2> &input_positions) {

  Vector2 input_start_position = (Vector2){100.0f, 200.0f};
  Vector2 next_input_position_distance = (Vector2){100.0f, 0.0f};
  Vector2 curr_position = input_start_position;

  for (size_t i = 0; i < INPUT_DEGREE; i++) {
    curr_position = Vector2Add(curr_position, next_input_position_distance);
    input_positions.push_back(curr_position);
  }
}

void CircuitSolver::firstAdderLayer(
    std::vector<Vector2> &first_adder_layer_positions) {

  Vector2 start_position = (Vector2){150.0f, 400.0f};
  Vector2 next_position_distance = (Vector2){100.0f, 0.0f};
  Vector2 curr_position = start_position;

  for (size_t i = 0; i < INPUT_DEGREE - 1; i++) {
    curr_position = Vector2Add(curr_position, next_position_distance);
    first_adder_layer_positions.push_back(curr_position);
  }
}

void CircuitSolver::solve() {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "circuit visualization");
  SetTargetFPS(SCREEN_FPS);

  std::vector<Vector2> input_positions;
  inputLayer(input_positions);

  std::vector<Vector2> first_adder_layer_positions;
  firstAdderLayer(first_adder_layer_positions);

  while (!WindowShouldClose()) {
    BeginDrawing();
    {
      ClearBackground(BLACK);
      {
        DrawCircleV({50.0f, 50.0f}, NODE_RADIUS, ORANGE);
        for (size_t i = 0; i < input_positions.size(); i++) {
          DrawCircle(input_positions[i].x, input_positions[i].y, NODE_RADIUS,
                     GetColor(0xcd5c5c));
        }

        for (size_t i = 0; i < first_adder_layer_positions.size(); i++) {
          DrawCircle(first_adder_layer_positions[i].x,
                     first_adder_layer_positions[i].y, NODE_RADIUS, RED);
        }

        for (size_t i = 0; i < first_adder_layer_positions.size(); i++) {
          DrawLineEx(input_positions[i], first_adder_layer_positions[i], 5.0f,
                     YELLOW);
        }
      }
    }
    EndDrawing();
  }
  CloseWindow();
}
