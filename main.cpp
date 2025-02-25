#include "raylib.h"
#include "Creature.h"
#include <vector>

int main() {
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "Creature Simulation");

    // Physics timestep (60 updates per second)
    const float fixedDeltaTime = 1.0f/60.0f;
    float accumulator = 0.0f;

    // Create initial creatures
    std::vector<Creature> creatures;
    for (int i = 0; i < 10; i++) {
        Vector2 pos = {
            (float)GetRandomValue(0, screenWidth),
            (float)GetRandomValue(0, screenHeight)
        };
        creatures.emplace_back(pos, 20.0f);
    }

    while (!WindowShouldClose()) {
        accumulator += GetFrameTime();

        while (accumulator >= fixedDeltaTime) {
            // Update all creatures
            for (auto& creature : creatures) {
                creature.Update(fixedDeltaTime, creatures);
            }
            
            // Remove dead creatures
            creatures.erase(
                std::remove_if(creatures.begin(), creatures.end(),
                    [](const Creature& c) { return !c.IsAlive(); }),
                creatures.end()
            );

            accumulator -= fixedDeltaTime;
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);
            
            // Draw all creatures
            for (const auto& creature : creatures) {
                creature.Draw();
            }
            
            DrawFPS(10, 10);
            DrawText(TextFormat("Creatures: %d", (int)creatures.size()), 
                    10, 30, 20, BLACK);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
