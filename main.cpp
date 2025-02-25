#include "raylib.h"
#include "Creature.h"
#include "Food.h"
#include "Constants.h"
#include <vector>

int main() {
    const int screenWidth = Constants::SCREEN_WIDTH;
    const int screenHeight = Constants::SCREEN_HEIGHT;
    InitWindow(screenWidth, screenHeight, "Creature Simulation");

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
