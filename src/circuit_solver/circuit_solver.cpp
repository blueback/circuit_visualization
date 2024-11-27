#include "circuit_solver/circuit_solver.hpp"
#include "ffmpeg_rendering/ffmpeg.hpp"

void IntegerFactorization::RegularAPCircuit::createCircuit(
    const uint32_t degree) {

  const uint32_t input_node = addNode(InputNodeType, 0);

  uint32_t prev_multiplier_output = 0;
  for (uint32_t i = 0; i < degree; i++) {
    const uint32_t constant = addNode(ConstantType, i + 1);
    const uint32_t adder = addNode(AdderType, 0);
    if (i == 0) {
      prev_multiplier_output = adder;
    } else {
      const uint32_t curr_multiplier = addNode(MultiplierType, 0);
      addEdge(prev_multiplier_output, curr_multiplier);
      addEdge(adder, curr_multiplier);
      prev_multiplier_output = curr_multiplier;
    }
    addEdge(constant, adder);
    addEdge(input_node, adder);
  }
}

void IntegerFactorization::Opt01Circuit::createCircuit(const uint32_t degree) {

  assert(degree > 2);
  std::vector<std::vector<uint32_t>> nodes;
  nodes.resize(degree);
  for (uint32_t i = 0; i < degree; i++) {
    nodes[i].resize(degree);
  }

  for (uint32_t i = 0; i < degree; i++) {
    const uint32_t input_node = addNode(InputNodeType, 0);
    nodes[0][i] = input_node;
  }

  for (uint32_t i = 0; i < degree; i++) {
    const uint32_t const_node = addNode(ConstantType, i);
    nodes[1][i] = const_node;
  }

  for (uint32_t i = 1; i < degree - 1; i++) {
    for (uint32_t j = 0; j < degree; j++) {
      const uint32_t adder = addNode(AdderType, 0);
      nodes[i][j] = adder;
      for (uint32_t k = 0; k < i; k++) {
        for (uint32_t l = 0; l < degree; l++) {
          addEdge(nodes[k][l], adder);
        }
      }
    }
  }

  for (uint32_t i = 0; i < degree; i++) {
    const uint32_t output = addNode(OutputNodeType, 0);
    nodes[degree - 1][i] = output;
    for (uint32_t k = 0; k < degree; k++) {
      addEdge(nodes[degree - 2][k], output);
    }
  }
  printf("created circuit\n");
}

void CircuitSolver::addOneCircuitToAnimate(CircuitModel *circuit) {
  float prev_end_time = 0.0f;
  if (_animators.size() != 0) {
    prev_end_time = _animators[_animators.size() - 1].getAnimationEndTime();
  }
  _circuits.push_back(circuit);
  _animators.push_back(
      CircuitAnimator(*circuit, SCREEN_RESOLUTION, prev_end_time));
}

void CircuitSolver::stackCircuitsToAnimate(void) {
  addOneCircuitToAnimate(new IntegerFactorization::RegularAPCircuit(8));
  addOneCircuitToAnimate(new IntegerFactorization::Opt01Circuit(4));
}

inline bool CircuitSolver::drawCircuits(const float time) {
  assert(_current_animator < _animators.size());
  if (_animators[_current_animator].updateCircuitAnimation(time)) {
    return true;
  } else {
    _current_animator++;
    if (_current_animator < _animators.size()) {
      return true;
    }
  }
  return false;
}

void CircuitSolver::solve() {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "circuit visualization");
  SetTargetFPS(SCREEN_FPS);

  // float zoom = 0.25f;
  float zoom = 0.95f;
  Camera2D camera = {.offset = {.x = SCREEN_WIDTH / 2 * (1.0f - zoom),
                                .y = SCREEN_HEIGHT / 2 * (1.0f - zoom)},
                     .target = {.x = 0, .y = 0},
                     .rotation = 0.0f,
                     .zoom = zoom};

  float curr_frame_time = 0.0f;

  stackCircuitsToAnimate();

  while (!WindowShouldClose()) {
    curr_frame_time += GetFrameTime();
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
      ClearBackground(DARKGRAY);
      BeginMode2D(camera);
      {
        DrawRectangleLinesEx(SCREEN_RECT, 3.0f, YELLOW);
        bool should_continue = drawCircuits(curr_frame_time);
        if (!should_continue) {
          break;
        }
      }
      EndMode2D();
    }
    EndDrawing();
  }
  CloseWindow();
}

void CircuitSolver::render_video() {

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "circuit visualization");
  SetTargetFPS(SCREEN_FPS);

  float curr_frame_time = 0.0f;

  stackCircuitsToAnimate();

  FFMPEG *ffmpeg = ffmpeg_start_rendering("output.mp4", SCREEN_WIDTH,
                                          SCREEN_HEIGHT, SCREEN_FPS);

  RenderTexture2D render_screen =
      LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);
  SetTraceLogLevel(LOG_WARNING);

  while (!WindowShouldClose()) {
    curr_frame_time += GetFrameTime();

    BeginDrawing();
    {
      BeginTextureMode(render_screen);
      {
        ClearBackground(DARKGRAY);
        bool should_continue = drawCircuits(curr_frame_time);
        if (!should_continue) {
          break;
        }
      }
      EndTextureMode();

      Image image = LoadImageFromTexture(render_screen.texture);
      if (!ffmpeg_send_frame_flipped(ffmpeg, image.data, image.width,
                                     image.height)) {
        ffmpeg_end_rendering(ffmpeg, true);
      }

      UnloadImage(image);
    }
    EndDrawing();
  }
  CloseWindow();

  ffmpeg_end_rendering(ffmpeg, false);
}
