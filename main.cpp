#include "raylib.h"
#include "Creature.h"
#include "Food.h"
#include "Constants.h"
#include <vector>

int main() {
    const int screenWidth = Constants::SCREEN_WIDTH;
    const int screenHeight = Constants::SCREEN_HEIGHT;
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);  // Make window resizable for macOS maximize
    InitWindow(screenWidth, screenHeight, "Creature Simulation");
    
    // Initialize camera
    // Helper functions for smooth camera movement
    auto Clamp = [](float value, float min, float max) -> float {
        if (value < min) return min;
        if (value > max) return max;
        return value;
    };
    
    auto Lerp = [](float start, float end, float amount) -> float {
        return start + amount * (end - start);
    };

    // Initialize camera
    Camera2D camera = {
        {screenWidth/2.0f, screenHeight/2.0f},  // offset
        {screenWidth/2.0f, screenHeight/2.0f},  // target
        0.0f,                                   // rotation
        1.0f                                    // zoom
    };
    
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
        Vector2 pos = {
            (float)GetRandomValue(0, screenWidth),
            (float)GetRandomValue(0, screenHeight)
        };
        creatures.emplace_back(pos, Constants::INITIAL_CREATURE_SIZE);
    }

    while (!WindowShouldClose()) {
        // Toggle fullscreen with F11
        if (IsKeyPressed(KEY_F11)) {
            ToggleFullscreen();
        }

        // Handle zoom
        float wheel = GetMouseWheelMove();
        if (wheel != 0) {
            targetZoom += wheel * 0.1f;
            targetZoom = Clamp(targetZoom, 0.1f, 3.0f);
        }
        
        // Smooth zoom interpolation
        camera.zoom = Lerp(camera.zoom, targetZoom, 0.1f);
        
        // Pan with middle mouse button or by dragging with left mouse button
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 delta = GetMouseDelta();
            camera.target.x -= (delta.x / camera.zoom);
            camera.target.y -= (delta.y / camera.zoom);
        }

        accumulator += GetFrameTime();

        while (accumulator >= fixedDeltaTime) {
            // Spawn food periodically
            foodSpawnTimer += fixedDeltaTime;
            if (foodSpawnTimer >= foodSpawnInterval) {
                // Spawn multiple food items each time
                for (int i = 0; i < Constants::FOOD_SPAWN_COUNT; i++) {
                    Vector2 foodPos = {
                        (float)GetRandomValue(0, screenWidth),
                        (float)GetRandomValue(0, screenHeight)
                    };
                    foods.emplace_back(foodPos);
                }
                foodSpawnTimer = 0;
            }
            
            // Update all creatures
            for (auto& creature : creatures) {
                creature.Update(fixedDeltaTime, creatures, foods);
            }
            
            // Remove consumed food
            foods.erase(
                std::remove_if(foods.begin(), foods.end(),
                    [](const Food& f) { return f.IsConsumed(); }),
                foods.end()
            );
            
            // Remove dead creatures
            creatures.erase(
                std::remove_if(creatures.begin(), creatures.end(),
                    [](const Creature& c) { return !c.IsAlive(); }),
                creatures.end()
            );

            accumulator -= fixedDeltaTime;
        }

        BeginDrawing();
            ClearBackground(BLACK);
            BeginMode2D(camera);
            
            // Draw world border using current window size
            DrawRectangleLinesEx(
                Rectangle{0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()},
                2,  // line thickness
                ColorAlpha(LIGHTGRAY, 0.5f)  // semi-transparent light gray
            );
            
            // Draw food
            for (const auto& food : foods) {
                food.Draw();
            }
            
            // Draw creatures
            for (const auto& creature : creatures) {
                creature.Draw();
            }
            
            EndMode2D();
            
            // Draw UI (not affected by camera)
            DrawFPS(10, 10);
            DrawText(TextFormat("Creatures: %d", (int)creatures.size()), 
                    10, 30, 20, WHITE);
            DrawText(TextFormat("Zoom: %.2fx", camera.zoom),
                    10, 50, 20, WHITE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
