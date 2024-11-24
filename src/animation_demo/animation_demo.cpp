#include "animation_demo/animation_demo.hpp"

void CircuitAnimationDemo::constantLayer(void) {

  Vector2 start_position = (Vector2){100.0f, 300.0f};
  Vector2 next_position_distance = (Vector2){200.0f, 0.0f};
  Vector2 curr_position = start_position;

  for (size_t i = 0; i < INPUT_DEGREE; i++) {
    curr_position = Vector2Add(curr_position, next_position_distance);
    constant_layer_positions.push_back(curr_position);
  }
}

void CircuitAnimationDemo::adderLayer(void) {

  Vector2 start_position = (Vector2){150.0f, 400.0f};
  Vector2 next_position_distance = (Vector2){200.0f, 0.0f};
  Vector2 curr_position = start_position;

  for (size_t i = 0; i < INPUT_DEGREE; i++) {
    curr_position = Vector2Add(curr_position, next_position_distance);
    adder_layer_positions.push_back(curr_position);
  }
}

void CircuitAnimationDemo::multiplierLayers(void) {

  Vector2 start_position = (Vector2){250.0f, 500.0f};
  Vector2 next_position_distance = (Vector2){200.0f, 100.0f};
  Vector2 curr_position = start_position;

  for (size_t i = 0; i < INPUT_DEGREE - 1; i++) {
    curr_position = Vector2Add(curr_position, next_position_distance);
    multiplier_layer_positions.push_back(curr_position);
  }
}

void CircuitAnimationDemo::drawCircuit(void) {
  DrawRectangleLinesEx(SCREEN_RECT, 3.0f, YELLOW);
  for (size_t i = 0; i < constant_layer_positions.size(); i++) {
    Vector2 top_mid_position =
        Vector2Add(constant_layer_positions[i], {100.0f, NODE_RADIUS});
    Vector2 bottom_mid_position =
        Vector2Add(constant_layer_positions[i], {100.0f, -(4 * NODE_RADIUS)});
    Vector2 points[4] = {input_position, top_mid_position, bottom_mid_position,
                         adder_layer_positions[i]};
    DrawSplineBezierCubic(points, 4, 3.0f, YELLOW);
  }

  for (size_t i = 0; i < adder_layer_positions.size(); i++) {
    DrawLineEx(constant_layer_positions[i], adder_layer_positions[i], 5.0f,
               YELLOW);
  }

  for (size_t i = 0; i < multiplier_layer_positions.size(); i++) {
    if (i == 0) {
      DrawLineEx(adder_layer_positions[i], multiplier_layer_positions[i], 3.0f,
                 YELLOW);
    } else {
      DrawLineEx(multiplier_layer_positions[i - 1],
                 multiplier_layer_positions[i], 3.0f, YELLOW);
    }
    DrawLineEx(adder_layer_positions[i + 1], multiplier_layer_positions[i],
               3.0f, YELLOW);
  }

  DrawCircleV(input_position, NODE_RADIUS, DARKGREEN);
  DrawText("x", input_position.x - 15, input_position.y - 25, 50.0f, YELLOW);

  for (size_t i = 0; i < constant_layer_positions.size(); i++) {
    DrawCircleV(constant_layer_positions[i], NODE_RADIUS, DARKGREEN);
    char buf[21];
    sprintf(buf, "%lu", i + 1);
    DrawText(buf, constant_layer_positions[i].x - 15,
             constant_layer_positions[i].y - 25, 50.0f, YELLOW);
  }

  for (size_t i = 0; i < adder_layer_positions.size(); i++) {
    DrawCircleV(adder_layer_positions[i], NODE_RADIUS, RED);
    DrawText("+", adder_layer_positions[i].x - 15,
             adder_layer_positions[i].y - 25, 50.0f, YELLOW);
  }

  for (auto multiplier_position : multiplier_layer_positions) {
    DrawCircleV(multiplier_position, NODE_RADIUS, BLUE);
    DrawText("*", multiplier_position.x - 15, multiplier_position.y - 25, 50.0f,
             YELLOW);
  }
}

void CircuitAnimationDemo::solve() {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "circuit visualization");
  SetTargetFPS(SCREEN_FPS);

  input_position = (Vector2){SCREEN_WIDTH / 2, 50.0f};
  constantLayer();
  adderLayer();
  multiplierLayers();

  float zoom = 0.25f;
  // float zoom = 0.95f;
  Camera2D camera = {.offset = {.x = SCREEN_WIDTH / 2 * (1.0f - zoom),
                                .y = SCREEN_HEIGHT / 2 * (1.0f - zoom)},
                     .target = {.x = 0, .y = 0},
                     .rotation = 0.0f,
                     .zoom = zoom};

  while (!WindowShouldClose()) {
    if (IsKeyDown(KEY_SPACE)) {
      camera.zoom += 0.01f;
    } else if (IsKeyDown(KEY_LEFT_SHIFT)) {
      camera.zoom -= 0.01f;
    }

    const float camera_offset_ratio = (1 - camera.zoom) / 2;
    camera.offset = Vector2Multiply(SCREEN_RESOLUTION,
                                    {camera_offset_ratio, camera_offset_ratio});

    BeginDrawing();
    {
      ClearBackground(BLACK);
      BeginMode2D(camera);
      {
        drawCircuit();
      }
      EndMode2D();
    }
    EndDrawing();
  }
  CloseWindow();
}
