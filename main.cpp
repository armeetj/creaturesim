#include "Constants.h"
#include "Creature.h"
#include "Food.h"
#include "raylib.h"
#include <cmath>
#include <vector>

int main() {
  const int screenWidth = Constants::SCREEN_WIDTH;
  const int screenHeight = Constants::SCREEN_HEIGHT;
  /*SetConfigFlags(FLAG_WINDOW_RESIZABLE);*/
  /*SetConfigFlags(FLAG_WINDOW_TOPMOST);*/
  InitWindow(screenWidth, screenHeight, "Creature Sim");

  // Initialize camera
  // Helper functions for smooth camera movement
  auto Clamp = [](float value, float min, float max) -> float {
    if (value < min)
      return min;
    if (value > max)
      return max;
    return value;
  };

  auto Lerp = [](float start, float end, float amount) -> float {
    return start + amount * (end - start);
  };

  // Initialize camera
  Camera2D camera = {
      {(float)screenWidth / 2.0f, (float)screenHeight / 2.0f}, // offset
      {0, 0}, // target (will be updated to follow creature)
      0.0f,   // rotation
      1.0f    // zoom
  };

  Creature *selectedCreature = nullptr;

  // For smooth zooming
  float targetZoom = 1.0f;

  // Physics timestep (60 updates per second)
  const float fixedDeltaTime = Constants::PHYSICS_TIMESTEP;
  float accumulator = 0.0f;

  std::vector<Creature> creatures;
  std::vector<Food> foods;

  float foodSpawnTimer = 0;
  const float foodSpawnInterval = Constants::FOOD_SPAWN_INTERVAL;
  for (int i = 0; i < Constants::INITIAL_CREATURE_COUNT; i++) {
    Vector2 pos = {(float)GetRandomValue(0, screenWidth),
                   (float)GetRandomValue(0, screenHeight)};
    creatures.emplace_back(pos, Constants::INITIAL_CREATURE_SIZE);
  }

  while (!WindowShouldClose()) {
    // Handle keyboard input
    if (IsKeyPressed(KEY_F)) {
      if (IsWindowFullscreen()) {
        ToggleFullscreen();
        SetWindowSize(screenWidth, screenHeight);
      } else {
        SetWindowSize(GetMonitorWidth(GetCurrentMonitor()),
                      GetMonitorHeight(GetCurrentMonitor()));
        ToggleFullscreen();
      }
    }

    if (IsKeyPressed(KEY_SPACE)) {
      camera.target = {0, 0};
      camera.zoom = 1.0f;
      targetZoom = 1.0f;
      camera.offset = {(float)GetScreenWidth() / 2.0f,
                       (float)GetScreenHeight() / 2.0f};
    }

    // Handle zoom
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
      targetZoom += wheel * 0.1f;
      targetZoom = Clamp(targetZoom, 0.1f, 3.0f);
    }

    // Smooth zoom interpolation
    camera.zoom = Lerp(camera.zoom, targetZoom, 0.1f);

    // Handle creature selection
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);

      // Deselect current creature
      if (selectedCreature) {
        selectedCreature->SetSelected(false);
        selectedCreature = nullptr;
      }

      // Check if clicked on a creature
      for (auto &creature : creatures) {
        Vector2 pos = creature.GetPosition();
        float dist = sqrt(pow(mouseWorldPos.x - pos.x, 2) +
                          pow(mouseWorldPos.y - pos.y, 2));
        if (dist < Constants::INITIAL_CREATURE_SIZE) {
          selectedCreature = &creature;
          creature.SetSelected(true);
          break;
        }
      }
    }

    // Pan with middle mouse button or left mouse button
    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE) ||
        IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
      Vector2 delta = GetMouseDelta();
      camera.target.x -= (delta.x / camera.zoom);
      camera.target.y -= (delta.y / camera.zoom);
    }

    // Update camera offset based on current window size
    camera.offset = {(float)GetScreenWidth() / 2.0f,
                     (float)GetScreenHeight() / 2.0f};

    // Smoother camera movement with exponential decay
    const float smoothFactor = 0.001f; // Adjust for more or less lag
    if (selectedCreature) {
      Vector2 pos = selectedCreature->GetPosition();
      camera.target.x += (pos.x - camera.target.x) * smoothFactor;
      camera.target.y += (pos.y - camera.target.y) * smoothFactor;
    }

    accumulator += GetFrameTime();

    while (accumulator >= fixedDeltaTime) {
      // Spawn food periodically
      foodSpawnTimer += fixedDeltaTime;
      if (foodSpawnTimer >= foodSpawnInterval) {
        // Spawn multiple food items each time
        for (int i = 0; i < Constants::FOOD_SPAWN_COUNT; i++) {
          Vector2 foodPos = {(float)GetRandomValue(0, screenWidth),
                             (float)GetRandomValue(0, screenHeight)};
          foods.emplace_back(foodPos);
        }
        foodSpawnTimer = 0;
      }

      // Update all creatures
      for (auto &creature : creatures) {
        creature.Update(fixedDeltaTime, creatures, foods, creatures);
      }

      // Remove consumed food
      foods.erase(std::remove_if(foods.begin(), foods.end(),
                                 [](const Food &f) { return f.IsConsumed(); }),
                  foods.end());

      // Remove dead creatures
      creatures.erase(
          std::remove_if(creatures.begin(), creatures.end(),
                         [](const Creature &c) { return !c.IsAlive(); }),
          creatures.end());

      accumulator -= fixedDeltaTime;
    }

    BeginDrawing();
    ClearBackground(Color{10, 10, 10, 255}); // Dark background
    BeginMode2D(camera);

    // Draw world border using current window size
    // Draw world border with gradient
    Color borderColor = ColorAlpha(LIGHTGRAY, 0.3f);
    DrawRectangleLinesEx(
        Rectangle{0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()}, 2,
        borderColor);

    // Draw food
    for (const auto &food : foods) {
      food.Draw();
    }

    // Sort creatures by age for rank
    std::vector<std::reference_wrapper<const Creature>> ranked_creatures(
        creatures.begin(), creatures.end());
    std::sort(ranked_creatures.begin(), ranked_creatures.end(),
              [](const Creature &a, const Creature &b) {
                return a.GetAge() > b.GetAge();
              });

    // Draw creatures with rank
    for (const auto &creature : creatures) {
      // Find creature's rank
      auto it = std::find_if(
          ranked_creatures.begin(), ranked_creatures.end(),
          [&creature](const std::reference_wrapper<const Creature> &ref) {
            return &ref.get() == &creature;
          });
      int rank = std::distance(ranked_creatures.begin(), it) + 1;
      creature.Draw(rank);
    }

    EndMode2D();

    // Draw UI (not affected by camera)
    DrawFPS(10, 10);
    DrawText(TextFormat("Zoom: %.2fx", camera.zoom), 10, 50, 20, WHITE);

    // Sort creatures by age
    std::vector<std::reference_wrapper<const Creature>> sorted_creatures(
        creatures.begin(), creatures.end());
    std::sort(sorted_creatures.begin(), sorted_creatures.end(),
              [](const Creature &a, const Creature &b) {
                return a.GetAge() > b.GetAge();
              });

    // Draw leaderboard
    const int BOARD_WIDTH = 250;
    const int BOARD_PADDING = 10;
    const int ENTRY_HEIGHT = 25;
    const int HEADER_HEIGHT = 50;
    const int numEntries = std::min(10, (int)sorted_creatures.size());
    const int BOARD_HEIGHT = HEADER_HEIGHT + (numEntries * ENTRY_HEIGHT);

    const int BOARD_X = GetScreenWidth() - BOARD_WIDTH - BOARD_PADDING;
    const int TEXT_X = BOARD_X + 10; // Left padding for text

    // Draw leaderboard background with gradient
    Color topColor = {20, 20, 40, 230};
    Color bottomColor = {40, 40, 60, 230};
    DrawRectangleGradientV(BOARD_X, BOARD_PADDING, BOARD_WIDTH, BOARD_HEIGHT,
                           topColor, bottomColor);

    // Draw border
    DrawRectangleLinesEx(Rectangle{(float)BOARD_X, (float)BOARD_PADDING,
                                   (float)BOARD_WIDTH, (float)BOARD_HEIGHT},
                         1, ColorAlpha(LIGHTGRAY, 0.3f));

    // Draw title
    DrawText("TOP CREATURES", TEXT_X, BOARD_PADDING + 5, 20, YELLOW);
    DrawText(TextFormat("Total: %d", (int)creatures.size()), TEXT_X,
             BOARD_PADDING + 28, 15, LIGHTGRAY);

    // Show top creatures
    for (int i = 0; i < numEntries; i++) {
      const auto &creature = sorted_creatures[i];

      // Creature rank and name
      DrawText(TextFormat("%d. %s", i + 1, creature.get().GetName().c_str()),
               TEXT_X, BOARD_PADDING + HEADER_HEIGHT + (i * ENTRY_HEIGHT), 15,
               WHITE);

      // Creature stats aligned to the right
      DrawText(TextFormat("H:%.0f E:%.0f", creature.get().GetHealth(),
                          creature.get().GetEnergy()),
               BOARD_X + BOARD_WIDTH -
                   80, // Adjust positioning for full width usage
               BOARD_PADDING + HEADER_HEIGHT + (i * ENTRY_HEIGHT), 12, WHITE);
    }
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
