#include "raylib_probe/raylib_probe.hpp"
#include "raylib.h"
#include "raymath.h"

void RaylibProbe::runRaylibProbe(void) {

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "circuit visualization");

  Texture2D dvd = LoadTexture("resources/cd_dvd_PNG102322.png");
  Shader shader = LoadShader(0, TextFormat("resources/wave.fs", 330));

  Camera2D camera = {{0, 0}, {0, 0}, 0, 0};
  camera.target = (Vector2){10.0f, 20.0f};
  camera.offset = (Vector2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
  camera.zoom = 1.0f;

  Vector2 dvd_position = {0};
  Vector2 dvd_velocity = {DVD_VELOCITY * DELTA_TIME, DVD_VELOCITY * DELTA_TIME};

  int secondsLoc = GetShaderLocation(shader, "seconds");
  int freqXLoc = GetShaderLocation(shader, "freqX");
  int freqYLoc = GetShaderLocation(shader, "freqY");
  int ampXLoc = GetShaderLocation(shader, "ampX");
  int ampYLoc = GetShaderLocation(shader, "ampY");
  int speedXLoc = GetShaderLocation(shader, "speedX");
  int speedYLoc = GetShaderLocation(shader, "speedY");

  // Shader uniform values that can be updated at any time
  float freqX = 25.0f;
  float freqY = 25.0f;
  float ampX = 5.0f;
  float ampY = 5.0f;
  float speedX = 8.0f;
  float speedY = 8.0f;

  float screenSize[2] = {(float)GetScreenWidth(), (float)GetScreenHeight()};
  SetShaderValue(shader, GetShaderLocation(shader, "size"), &screenSize,
                 SHADER_UNIFORM_VEC2);
  SetShaderValue(shader, freqXLoc, &freqX, SHADER_UNIFORM_FLOAT);
  SetShaderValue(shader, freqYLoc, &freqY, SHADER_UNIFORM_FLOAT);
  SetShaderValue(shader, ampXLoc, &ampX, SHADER_UNIFORM_FLOAT);
  SetShaderValue(shader, ampYLoc, &ampY, SHADER_UNIFORM_FLOAT);
  SetShaderValue(shader, speedXLoc, &speedX, SHADER_UNIFORM_FLOAT);
  SetShaderValue(shader, speedYLoc, &speedY, SHADER_UNIFORM_FLOAT);

  float seconds = 0.0f;

  SetTargetFPS(SCREEN_FPS);

  while (!WindowShouldClose()) {
    seconds += GetFrameTime();

    SetShaderValue(shader, secondsLoc, &seconds, SHADER_UNIFORM_FLOAT);

    if (dvd_position.y + dvd.height * DVD_SCALING > SCREEN_HEIGHT) {
      dvd_velocity.y = -dvd_velocity.y;
    } else if (dvd_position.y < 0) {
      dvd_velocity.y = -dvd_velocity.y;
    }

    if (dvd_position.x + dvd.width * DVD_SCALING > SCREEN_WIDTH) {
      dvd_velocity.x = -dvd_velocity.x;
    } else if (dvd_position.x < 0) {
      dvd_velocity.x = -dvd_velocity.x;
    }

    dvd_position = Vector2Add(dvd_position, dvd_velocity);
    if (IsKeyDown(KEY_RIGHT)) {
      camera.target.x += 2;
    } else if (IsKeyDown(KEY_LEFT)) {
      camera.target.x -= 2;
    } else if (IsKeyDown(KEY_UP)) {
      camera.target.y -= 2;
    } else if (IsKeyDown(KEY_DOWN)) {
      camera.target.y += 2;
    } else if (IsKeyDown(KEY_SPACE)) {
      camera.zoom -= 0.01f;
    } else if (IsKeyDown(KEY_LEFT_SHIFT)) {
      camera.zoom += 0.01f;
    }
    BeginDrawing();
    {
      ClearBackground(BLACK);
      BeginMode2D(camera);
      {
        BeginShaderMode(shader);
        {
          DrawCircle(SCREEN_WIDTH / 3, SCREEN_HEIGHT / 3, 30.0, RED);
          DrawTextureEx(dvd, dvd_position, 0.0f, DVD_SCALING,
                        GetColor(0xFF0000FF));
        }
        EndShaderMode();

        // DrawCircle(2 * SCREEN_WIDTH / 3, 2 * SCREEN_HEIGHT / 3, 30.0, BLUE);
        // DrawCircle(2 * SCREEN_WIDTH / 3, SCREEN_HEIGHT / 3, 30.0, GREEN);
        // DrawCircle(SCREEN_WIDTH / 3, 2 * SCREEN_HEIGHT / 3, 30.0, YELLOW);
      }
      EndMode2D();
    }
    EndDrawing();
  }

  CloseWindow();
}
