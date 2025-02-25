#include "raylib.h"
#include "Creature.h"
#include "Food.h"
#include <vector>

int main() {
    const int screenWidth = 1200;
    const int screenHeight = 800;
    InitWindow(screenWidth, screenHeight, "Creature Simulation");

    // Physics timestep (60 updates per second)
    const float fixedDeltaTime = 1.0f/60.0f;
    float accumulator = 0.0f;

    std::vector<Creature> creatures;
    std::vector<Food> foods;
    
    float foodSpawnTimer = 0;
    const float foodSpawnInterval = 1.0f;
    for (int i = 0; i < 30; i++) {
        Vector2 pos = {
            (float)GetRandomValue(0, screenWidth),
            (float)GetRandomValue(0, screenHeight)
        };
        creatures.emplace_back(pos, 10.0f);
    }

    while (!WindowShouldClose()) {
        accumulator += GetFrameTime();

        while (accumulator >= fixedDeltaTime) {
            // Spawn food periodically
            foodSpawnTimer += fixedDeltaTime;
            if (foodSpawnTimer >= foodSpawnInterval) {
                Vector2 foodPos = {
                    (float)GetRandomValue(0, screenWidth),
                    (float)GetRandomValue(0, screenHeight)
                };
                foods.emplace_back(foodPos);
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
            
            // Draw food
            for (const auto& food : foods) {
                food.Draw();
            }
            
            // Draw creatures
            for (const auto& creature : creatures) {
                creature.Draw();
            }
            
            DrawFPS(10, 10);
            DrawText(TextFormat("Creatures: %d", (int)creatures.size()), 
                    10, 30, 20, WHITE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
