#include "raylib.h"
#include "Creature.h"
#include "Food.h"
#include "Constants.h"
#include <vector>
#include <cmath>

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
        {(float)screenWidth/2.0f, (float)screenHeight/2.0f},  // offset
        {0, 0},  // target (will be updated to follow creature)
        0.0f,    // rotation
        1.0f     // zoom
    };
    
    Creature* selectedCreature = nullptr;
    
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
        // Toggle fullscreen with F11 or F
        if (IsKeyPressed(KEY_F11) || IsKeyPressed(KEY_F)) {
            ToggleFullscreen();
        }

        // Reset view with spacebar
        if (IsKeyPressed(KEY_SPACE)) {
            camera.target = {0, 0};
            targetZoom = 1.0f;
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
            for (auto& creature : creatures) {
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
        if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE) || IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 delta = GetMouseDelta();
            camera.target.x -= (delta.x / camera.zoom);
            camera.target.y -= (delta.y / camera.zoom);
        }
        
        // Update camera offset based on current window size
        camera.offset = {(float)GetScreenWidth()/2.0f, (float)GetScreenHeight()/2.0f};
        
        // Update camera to follow selected creature
        if (selectedCreature) {
            Vector2 pos = selectedCreature->GetPosition();
            // Smoother camera following with variable speed based on distance
            float dx = pos.x - camera.target.x;
            float dy = pos.y - camera.target.y;
            float dist = sqrt(dx*dx + dy*dy);
            float speed = Clamp(dist / 500.0f, 0.02f, 0.1f);  // Faster when far, slower when close
            
            camera.target.x = Lerp(camera.target.x, pos.x, speed);
            camera.target.y = Lerp(camera.target.y, pos.y, speed);
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
                    
            // Draw leaderboard
            const int BOARD_WIDTH = 200;
            const int BOARD_PADDING = 10;
            DrawRectangle(GetScreenWidth() - BOARD_WIDTH - BOARD_PADDING, 
                         BOARD_PADDING, 
                         BOARD_WIDTH, 
                         120, 
                         ColorAlpha(BLACK, 0.7f));
            
            DrawText("TOP CREATURES", 
                    GetScreenWidth() - BOARD_WIDTH, 
                    BOARD_PADDING + 5, 
                    20, YELLOW);
                    
            // Sort creatures by health + energy
            std::vector<std::reference_wrapper<const Creature>> sorted_creatures(creatures.begin(), creatures.end());
            std::sort(sorted_creatures.begin(), sorted_creatures.end(),
                     [](const Creature& a, const Creature& b) {
                         return (a.GetHealth() + a.GetEnergy()) > (b.GetHealth() + b.GetEnergy());
                     });
            
            // Show top 3
            for (int i = 0; i < std::min(3, (int)sorted_creatures.size()); i++) {
                const auto& creature = sorted_creatures[i];
                DrawText(TextFormat("%d. %s", i + 1, creature.get().GetName().c_str()),
                        GetScreenWidth() - BOARD_WIDTH,
                        BOARD_PADDING + 30 + (i * 25),
                        15, WHITE);
                DrawText(TextFormat("H:%.0f E:%.0f", 
                        creature.get().GetHealth(),
                        creature.get().GetEnergy()),
                        GetScreenWidth() - BOARD_WIDTH + 120,
                        BOARD_PADDING + 30 + (i * 25),
                        12, WHITE);
            }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
