#include "Constants.h"
#include "Creature.h"
#include "Food.h"
#include "raylib.h"
#include <cmath>
#include <string>
#include <vector>

float simulationSpeed = 1.0f; // Global simulation speed multiplier

void DrawGameOverScreen(float totalAge, int totalCreatures) {
  BeginDrawing();
  ClearBackground(BLACK);

  // Title
  DrawText("GAME OVER", GetScreenWidth() / 2 - MeasureText("GAME OVER", 80) / 2,
           GetScreenHeight() / 2 - 100, 80, RED);

  // Simulation stats
  std::string statsText = TextFormat("Simulation Duration: %.1f seconds\n"
                                     "Total Creatures: %d",
                                     totalAge, totalCreatures);
  DrawText(statsText.c_str(),
           GetScreenWidth() / 2 - MeasureText(statsText.c_str(), 20) / 2,
           GetScreenHeight() / 2 + 50, 20, WHITE);

  // Restart instructions
  DrawText("Press ENTER to restart",
           GetScreenWidth() / 2 - MeasureText("Press ENTER to restart", 20) / 2,
           GetScreenHeight() / 2 + 150, 20, GRAY);

  EndDrawing();
}

int main() {
  const int screenWidth = Constants::SCREEN_WIDTH;
  const int screenHeight = Constants::SCREEN_HEIGHT;
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  SetConfigFlags(FLAG_WINDOW_TOPMOST);
  InitWindow(screenWidth, screenHeight, "Creature Sim");

  bool gameOver = false;
  float totalSimulationAge = 0.0f;
  int totalCreaturesEverLived = 0;
  float totalSimulationTime = 0.0f; // Track total simulation time

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

  while (!WindowShouldClose() && !creatures.empty()) {
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
      // Deselect any currently selected creature
      if (selectedCreature) {
        selectedCreature->SetSelected(false);
        selectedCreature = nullptr;
      }

      // Calculate center of creatures' boundary
      if (!creatures.empty()) {
        float minX = creatures[0].GetPosition().x;
        float maxX = minX;
        float minY = creatures[0].GetPosition().y;
        float maxY = minY;

        for (const auto &creature : creatures) {
          Vector2 pos = creature.GetPosition();
          minX = std::min(minX, pos.x);
          maxX = std::max(maxX, pos.x);
          minY = std::min(minY, pos.y);
          maxY = std::max(maxY, pos.y);
        }

        Vector2 centerPos = {(minX + maxX) / 2.0f, (minY + maxY) / 2.0f};

        camera.target = centerPos;
        camera.zoom = 1.0f;
        targetZoom = 1.0f;
        camera.offset = {(float)GetScreenWidth() / 2.0f,
                         (float)GetScreenHeight() / 2.0f};
      }
    }

    // Handle zoom
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
      targetZoom += wheel * 0.1f;
      targetZoom = Clamp(targetZoom, 0.5f, 5.0f);
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
          // Deselect any previously selected creature
          for (auto &otherCreature : creatures) {
            otherCreature.SetSelected(false);
          }

          // Select the clicked creature
          selectedCreature = &creature;
          creature.SetSelected(true);
          break;
        }
      }
    }

    // Navigate between creatures using arrow keys and number keys
    if (!creatures.empty()) {
      // Sort creatures by age for consistent navigation
      std::vector<std::reference_wrapper<Creature>> sorted_creatures(
          creatures.begin(), creatures.end());
      std::sort(sorted_creatures.begin(), sorted_creatures.end(),
                [](const Creature &a, const Creature &b) {
                  return a.GetAge() > b.GetAge();
                });

      // Number key selection (1-9)
      for (int key = KEY_ONE; key <= KEY_NINE; key++) {
        if (IsKeyPressed(key)) {
          int index = key - KEY_ONE;

          // Deselect current creature
          if (selectedCreature) {
            selectedCreature->SetSelected(false);
          }

          // Select creature by leaderboard position
          if (index < sorted_creatures.size()) {
            selectedCreature = &(sorted_creatures[index].get());
            selectedCreature->SetSelected(true);
          }
        }
      }

      if (IsKeyPressed(KEY_RIGHT)) {
        // Find current creature's index
        auto it = std::find_if(
            sorted_creatures.begin(), sorted_creatures.end(),
            [&selectedCreature](const std::reference_wrapper<Creature> &ref) {
              return &ref.get() == selectedCreature;
            });

        // Deselect current creature
        if (selectedCreature) {
          selectedCreature->SetSelected(false);
        }

        // Select next creature, wrap around if at end
        if (it != sorted_creatures.end()) {
          auto nextIt = std::next(it);
          if (nextIt == sorted_creatures.end()) {
            nextIt = sorted_creatures.begin();
          }
          selectedCreature = &(nextIt->get());
          selectedCreature->SetSelected(true);
        } else if (!sorted_creatures.empty()) {
          // Fallback if something goes wrong
          selectedCreature = &(sorted_creatures.front().get());
          selectedCreature->SetSelected(true);
        }
      }

      if (IsKeyPressed(KEY_LEFT)) {
        // Find current creature's index
        auto it = std::find_if(
            sorted_creatures.begin(), sorted_creatures.end(),
            [&selectedCreature](const std::reference_wrapper<Creature> &ref) {
              return &ref.get() == selectedCreature;
            });

        // Deselect current creature
        if (selectedCreature) {
          selectedCreature->SetSelected(false);
        }

        // Select previous creature, wrap around if at beginning
        if (it != sorted_creatures.end()) {
          if (it == sorted_creatures.begin()) {
            it = std::prev(sorted_creatures.end());
          } else {
            it = std::prev(it);
          }
          selectedCreature = &(it->get());
          selectedCreature->SetSelected(true);
        } else if (!sorted_creatures.empty()) {
          // Fallback if something goes wrong
          selectedCreature = &(sorted_creatures.front().get());
          selectedCreature->SetSelected(true);
        }
      }
    }

    // Track if user is manually panning
    static bool isManualPan = false;

    // Pan with middle mouse button or left mouse button
    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE) ||
        IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
      Vector2 delta = GetMouseDelta();
      camera.target.x -= (delta.x / camera.zoom);
      camera.target.y -= (delta.y / camera.zoom);
      isManualPan = true;
    }

    // Reset manual pan when space is pressed
    if (IsKeyPressed(KEY_SPACE)) {
      isManualPan = false;
    }

    // Update camera offset based on current window size
    camera.offset = {(float)GetScreenWidth() / 2.0f,
                     (float)GetScreenHeight() / 2.0f};

    // Calculate center of creatures
    if (!creatures.empty() && !isManualPan) {
      float minX = creatures[0].GetPosition().x;
      float maxX = minX;
      float minY = creatures[0].GetPosition().y;
      float maxY = minY;

      for (const auto &creature : creatures) {
        Vector2 pos = creature.GetPosition();
        minX = std::min(minX, pos.x);
        maxX = std::max(maxX, pos.x);
        minY = std::min(minY, pos.y);
        maxY = std::max(maxY, pos.y);
      }

      Vector2 centerPos = {(minX + maxX) / 2.0f, (minY + maxY) / 2.0f};

      // Smoother camera movement with exponential decay
      const float smoothFactor = 0.01f; // Adjust for more or less lag
      const float CAMERA_SMALL_MOVE_THRES =
          200; // largest number of pixels for small camera move

      if (selectedCreature) {
        Vector2 pos = selectedCreature->GetPosition();
        float dx = pos.x - camera.target.x;
        float dy = pos.y - camera.target.y;

        // Adjust for fullscreen
        if (IsWindowFullscreen()) {
          camera.target.x = centerPos.x;
          camera.target.y = centerPos.y;
        } else {
          if (abs(dx) < CAMERA_SMALL_MOVE_THRES) {
            camera.target.x += dx * smoothFactor * 0.1;
          } else {
            camera.target.x += dx * smoothFactor;
          }
          if (abs(dy) < CAMERA_SMALL_MOVE_THRES) {
            camera.target.y += dy * smoothFactor * 0.1;
          } else {
            camera.target.y += dy * smoothFactor;
          }
        }
      } else {
        // If no creature selected, center on the overall creature group
        camera.target.x = centerPos.x;
        camera.target.y = centerPos.y;
      }
    }

    accumulator += GetFrameTime();

    while (accumulator >= fixedDeltaTime) {
      // Accumulate total simulation time
      totalSimulationTime += fixedDeltaTime * simulationSpeed;

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

      // Handle simulation speed control with more consistent key handling
      if (IsKeyDown(KEY_UP)) {
        simulationSpeed *= 1.05f; // Gradual increase
        simulationSpeed = Clamp(simulationSpeed, 0.125f, 100.0f);
      }
      if (IsKeyDown(KEY_DOWN)) {
        simulationSpeed /= 1.05f; // Gradual decrease
        simulationSpeed = Clamp(simulationSpeed, 0.125f, 100.0f);
      }

      // Precise speed reset
      if (IsKeyPressed(KEY_R)) {
        simulationSpeed = 1.0f;
      }

      // Update all creatures
      for (auto &creature : creatures) {
        creature.Update(fixedDeltaTime * simulationSpeed, creatures, foods,
                        creatures);
      }

      // Remove consumed food
      foods.erase(std::remove_if(foods.begin(), foods.end(),
                                 [](const Food &f) { return f.IsConsumed(); }),
                  foods.end());

      // Remove dead creatures and track total creatures
      creatures.erase(
          std::remove_if(creatures.begin(), creatures.end(),
                         [](const Creature &c) { return !c.IsAlive(); }),
          creatures.end());

      // Update total creatures ever lived during creature updates
      for (const auto &creature : creatures) {
        if (creature.GetAge() <= fixedDeltaTime * simulationSpeed) {
          totalCreaturesEverLived++;
        }
      }

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

    // Draw grid dots
    int gridSpacing = 50; // Adjust for dot density
    Color dotColor = ColorAlpha(GRAY, 0.2f);
    for (int x = 0; x < GetScreenWidth(); x += gridSpacing) {
      for (int y = 0; y < GetScreenHeight(); y += gridSpacing) {
        DrawCircle(x, y, 2, dotColor);
      }
    }

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
      creature.Draw(rank, creatures);
    }

    EndMode2D();

    // Draw UI (not affected by camera)
    DrawFPS(10, 10);
    DrawText(TextFormat("Zoom: %.2fx", camera.zoom), 10, 50, 20, WHITE);
    DrawText(TextFormat("Sim Speed: %.2fx", simulationSpeed), 10, 70, 20,
             DARKGRAY);

    // Draw keybinds
    const int KEYBIND_Y = GetScreenHeight() - 250;
    const int KEYBIND_X = 10;
    const int KEYBIND_LINE_HEIGHT = 20;
    const int KEYBIND_FONT_SIZE = 15;
    Color keybindColor = LIGHTGRAY;

    DrawText("KEYBINDS:", KEYBIND_X, KEYBIND_Y, KEYBIND_FONT_SIZE + 2, YELLOW);
    DrawText("SPACE: Reset Camera", KEYBIND_X, KEYBIND_Y + KEYBIND_LINE_HEIGHT,
             KEYBIND_FONT_SIZE, keybindColor);
    DrawText("F: Toggle Fullscreen", KEYBIND_X,
             KEYBIND_Y + KEYBIND_LINE_HEIGHT * 2, KEYBIND_FONT_SIZE,
             keybindColor);
    DrawText("Mouse Wheel: Zoom", KEYBIND_X,
             KEYBIND_Y + KEYBIND_LINE_HEIGHT * 3, KEYBIND_FONT_SIZE,
             keybindColor);
    DrawText("Middle Mouse: Pan", KEYBIND_X,
             KEYBIND_Y + KEYBIND_LINE_HEIGHT * 4, KEYBIND_FONT_SIZE,
             keybindColor);
    DrawText("Left/Right: Select Creature", KEYBIND_X,
             KEYBIND_Y + KEYBIND_LINE_HEIGHT * 5, KEYBIND_FONT_SIZE,
             keybindColor);
    DrawText("1-9: Select Top Creatures", KEYBIND_X,
             KEYBIND_Y + KEYBIND_LINE_HEIGHT * 6, KEYBIND_FONT_SIZE,
             keybindColor);
    DrawText("UP/DOWN: Change Sim Speed", KEYBIND_X,
             KEYBIND_Y + KEYBIND_LINE_HEIGHT * 7, KEYBIND_FONT_SIZE,
             keybindColor);
    DrawText("R: Reset Sim Speed", KEYBIND_X,
             KEYBIND_Y + KEYBIND_LINE_HEIGHT * 8, KEYBIND_FONT_SIZE,
             keybindColor);

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

  // Game over handling
  if (creatures.empty()) {
    gameOver = true;

    // Create ranked_creatures vector if creatures is empty
    std::vector<std::reference_wrapper<const Creature>> ranked_creatures;
    totalSimulationAge = totalSimulationTime; // Use total simulation time
  }

  // Game over screen and restart logic
  while (gameOver) {
    DrawGameOverScreen(totalSimulationAge, totalCreaturesEverLived);

    // Restart option
    if (IsKeyPressed(KEY_ENTER)) {
      // Reset everything
      creatures.clear();
      foods.clear();
      selectedCreature = nullptr;

      // Repopulate
      for (int i = 0; i < Constants::INITIAL_CREATURE_COUNT; i++) {
        Vector2 pos = {(float)GetRandomValue(0, screenWidth),
                       (float)GetRandomValue(0, screenHeight)};
        creatures.emplace_back(pos, Constants::INITIAL_CREATURE_SIZE);
      }

      // Reset simulation variables
      simulationSpeed = 1.0f;
      totalSimulationAge = 0.0f;
      totalSimulationTime = 0.0f; // Reset total simulation time
      totalCreaturesEverLived = 0;
      gameOver = false;
    }

    // Allow quitting from game over screen
    if (WindowShouldClose()) {
      CloseWindow();
      return 0;
    }
  }

  CloseWindow();
  return 0;
}
