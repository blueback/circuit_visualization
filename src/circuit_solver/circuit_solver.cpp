#include "circuit_solver/circuit_solver.hpp"
#include "ffmpeg_rendering/ffmpeg.hpp"

void ExampleCircuit001::createCircuit(void) {
  // x^2 + 2x + 1
  const uint32_t input = addNode(InputNodeType, 0);
  const uint32_t m1 = addNode(MultiplierType, 0);
  const uint32_t m2 = addNode(MultiplierType, 0);
  const uint32_t c1 = addNode(ConstantType, 2);
  const uint32_t c2 = addNode(ConstantType, 1);
  const uint32_t a1 = addNode(AdderType, 0);
  const uint32_t a2 = addNode(AdderType, 0);
  const uint32_t o1 = addNode(OutputNodeType, 0);
  addEdge(input, m1);
  addEdge(input, m1);
  addEdge(input, m2);
  addEdge(c1, m2);
  addEdge(m1, a1);
  addEdge(m2, a1);
  addEdge(a1, a2);
  addEdge(c2, a2);
  addEdge(a2, o1);
}

void ExampleCircuit002::createCircuit(void) {
  // x^5 + 2x^4 + 3x^3 + 4x^2 + 5x + 6
  const uint32_t input = addNode(InputNodeType, 0);
  uint32_t prev_m(0);
  for (uint32_t i = 0; i < 6; i++) {
    const uint32_t c = addNode(ConstantType, i + 1);
    uint32_t curr_m(0);
    if ((i + 1) == 6) {
      curr_m = c;
    } else {
      const uint32_t m = addNode(MultiplierType, 0);
      addEdge(input, m);
      addEdge(c, m);
      curr_m = m;
    }
    for (uint32_t j = 0; j < 5 - i; j++) {
      const uint32_t m = addNode(MultiplierType, 0);
      addEdge(input, m);
      addEdge(curr_m, m);
      curr_m = m;
    }

    if (i == 0) {
      prev_m = curr_m;
    } else {
      const uint32_t a = addNode(AdderType, 0);
      addEdge(prev_m, a);
      addEdge(curr_m, a);
      prev_m = a;
    }
  }
  const uint32_t output = addNode(OutputNodeType, 0);
  addEdge(prev_m, output);
}

void ExampleCircuit003::createCircuit(void) {
  // x ^ 3
  const uint32_t input = addNode(InputNodeType, 0);
  const uint32_t m1 = addNode(MultiplierType, 0);
  const uint32_t m2 = addNode(MultiplierType, 0);
  const uint32_t output = addNode(OutputNodeType, 0);
  addEdge(input, m1);
  addEdge(input, m1);
  addEdge(input, m2);
  addEdge(m1, m2);
  addEdge(m2, output);
}

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
}

void CircuitSolver::addOneCircuitToAnimate(CircuitModel *circuit) {
  float prev_end_time = 0.0f;
  if (_animators.size() != 0) {
    prev_end_time = _animators[_animators.size() - 1].getAnimationEndTime();
  }
  _circuits.push_back(circuit);
  _animators.push_back(CircuitAnimator(*circuit, SCREEN_RESOLUTION,
                                       getBackgroundTopColor(), SCREEN_FPS,
                                       prev_end_time));
}

void CircuitSolver::stackCircuitsToAnimate(void) {
  addOneCircuitToAnimate(new ExampleCircuit001());
  addOneCircuitToAnimate(new ExampleCircuit002());
  addOneCircuitToAnimate(new ExampleCircuit003());
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
        drawVideoBackground(true);
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

static bool isRectangleInside(Rectangle screen_rect, Rectangle target_rect) {
  Rectangle coll_rect = GetCollisionRec(screen_rect, target_rect);
  if (coll_rect.x != target_rect.x) {
    return false;
  }
  if (coll_rect.y != target_rect.y) {
    return false;
  }
  if (coll_rect.width != target_rect.width) {
    return false;
  }
  if (coll_rect.height != target_rect.height) {
    return false;
  }
  return true;
}

static float getMaximumZoom(Rectangle screen_rect, Rectangle target_rect) {
  float height_zoom = screen_rect.height / target_rect.height;
  float width_zoom = screen_rect.width / target_rect.width;
  if (height_zoom > width_zoom) {
    return height_zoom;
  } else {
    return width_zoom;
  }
}

static Rectangle getZoomTargetRectangle(Rectangle screen_rect,
                                        Vector2 target_center,
                                        float zoom_factor) {
  Rectangle target_rect;
  target_rect.width = screen_rect.width / zoom_factor;
  target_rect.height = screen_rect.height / zoom_factor;
  Vector2 top_left = Vector2Subtract(
      target_center, {.x = target_rect.width / 2, .y = target_rect.height / 2});
  target_rect.x = top_left.x;
  target_rect.y = top_left.y;
  return target_rect;
}

static Camera2D getCurrCameraZoomTo(Rectangle screen_rect,
                                    Rectangle target_rect, float start_time,
                                    float end_time, float curr_time) {
  assert(start_time < end_time);
  curr_time = Clamp(curr_time, start_time, end_time);

  assert(isRectangleInside(screen_rect, target_rect));

  float default_zoom = 1.0f;
  float final_zoom = getMaximumZoom(screen_rect, target_rect);
  float offset_ratio = -final_zoom / (final_zoom - 1.0f);

  float curr_zoom_slop = Remap(curr_time, start_time, end_time,
                               logf(default_zoom), logf(final_zoom));

  float curr_zoom = expf(curr_zoom_slop);

  Vector2 curr_offset = {.x = offset_ratio * (curr_zoom - 1.0f) * target_rect.x,
                         .y =
                             offset_ratio * (curr_zoom - 1.0f) * target_rect.y};

  Camera2D curr_camera = {.offset = {.x = curr_offset.x, .y = curr_offset.y},
                          .target = {.x = 0.0f, .y = 0.0f},
                          .rotation = 0.0f,
                          .zoom = curr_zoom};

  return curr_camera;
}

static MAYBE_UNUSED Camera2D getCurrCameraZoomTo(
    Rectangle screen_rect, Vector2 target_center, float zoom_factor,
    float start_time, float end_time, float curr_time) {
  Rectangle target_rect =
      getZoomTargetRectangle(screen_rect, target_center, zoom_factor);

  return getCurrCameraZoomTo(screen_rect, target_rect, start_time, end_time,
                             curr_time);
}

static Camera2D getCurrCameraZoomFrom(Rectangle screen_rect,
                                      Rectangle target_rect, float start_time,
                                      float end_time, float curr_time) {
  assert(start_time < end_time);
  curr_time = Clamp(curr_time, start_time, end_time);

  assert(isRectangleInside(screen_rect, target_rect));

  float default_zoom = 1.0f;
  float initial_zoom = getMaximumZoom(screen_rect, target_rect);
  float offset_ratio = -initial_zoom / (initial_zoom - 1.0f);

  float curr_zoom_slop = Remap(curr_time, start_time, end_time,
                               logf(initial_zoom), logf(default_zoom));

  float curr_zoom = expf(curr_zoom_slop);

  Vector2 curr_offset = {.x = offset_ratio * (curr_zoom - 1.0f) * target_rect.x,
                         .y =
                             offset_ratio * (curr_zoom - 1.0f) * target_rect.y};

  Camera2D curr_camera = {.offset = {.x = curr_offset.x, .y = curr_offset.y},
                          .target = {.x = 0.0f, .y = 0.0f},
                          .rotation = 0.0f,
                          .zoom = curr_zoom};

  return curr_camera;
}

static MAYBE_UNUSED Camera2D getCurrCameraZoomFrom(
    Rectangle screen_rect, Vector2 target_center, float zoom_factor,
    float start_time, float end_time, float curr_time) {
  Rectangle target_rect =
      getZoomTargetRectangle(screen_rect, target_center, zoom_factor);

  return getCurrCameraZoomFrom(screen_rect, target_rect, start_time, end_time,
                               curr_time);
}

void CircuitSolver::drawVideoBackground(const bool use_mp) {
  // Color apap_color = ColorFromHSV(277, 0.35f, 0.57f);
  // Color mp_color = DARKBLUE;
  // Color bottom_color = use_mp ? mp_color : apap_color;
  // DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLUE,
  // bottom_color);
  Color c1 = getBackgroundTopColor();
  Color c2 = getBackgroundBottomColor();
  DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, c1, c2);
}

void CircuitSolver::render_video() {

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "circuit visualization");
  SetTargetFPS(SCREEN_FPS);

  float curr_frame_time = 0.0f;

#if 0
  Vector2 zoom_target_center = {.x = SCREEN_WIDTH / 5 * 3,
                                .y = SCREEN_HEIGHT / 6 * 2};
  float zoom_factor = 15.0f;
  Rectangle zoom_target_rect =
      getZoomTargetRectangle(SCREEN_RECT, zoom_target_center, zoom_factor);
  float zoom_in_start_time = 2.0f;
  float zoom_in_end_time = 10.0f;
  float zoom_out_start_time = 12.0f;
  float zoom_out_end_time = 20.0f;
#endif

  Camera2D camera = {.offset = {.x = 0.0f, .y = 0.0f},
                     .target = {.x = 0.0f, .y = 0.0f},
                     .rotation = 0.0f,
                     .zoom = 1.0f};

  stackCircuitsToAnimate();

  FFMPEG *ffmpeg = ffmpeg_start_rendering("output.mp4", SCREEN_WIDTH,
                                          SCREEN_HEIGHT, SCREEN_FPS);

  RenderTexture2D render_screen =
      LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);
  SetTraceLogLevel(LOG_WARNING);

  while (!WindowShouldClose()) {
    curr_frame_time += GetFrameTime();

#if 0
    if (curr_frame_time < zoom_out_start_time) {
      camera = getCurrCameraZoomTo(SCREEN_RECT, zoom_target_center, zoom_factor,
                                   zoom_in_start_time, zoom_in_end_time,
                                   curr_frame_time);
    } else {
      camera = getCurrCameraZoomFrom(SCREEN_RECT, zoom_target_center,
                                     zoom_factor, zoom_out_start_time,
                                     zoom_out_end_time, curr_frame_time);
    }
#endif

    BeginDrawing();
    {
      BeginTextureMode(render_screen);
      {
        BeginMode2D(camera);
        {
          ClearBackground(DARKGRAY);
          drawVideoBackground(true);
#if 0
          DrawRectangleLinesEx(zoom_target_rect, 1.0f, ORANGE);
#endif
          bool should_continue = drawCircuits(curr_frame_time);
          if (!should_continue) {
            break;
          }
        }
        EndMode2D();
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
