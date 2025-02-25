#include "raylib.h"

int main() {
    // Initialize window
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "Bouncing Ball Game");

    // Physics timestep (60 updates per second)
    const float fixedDeltaTime = 1.0f/60.0f;
    float accumulator = 0.0f;

    // Ball properties
    Vector2 ballPosition = { screenWidth/2.0f, screenHeight/2.0f };
    Vector2 ballSpeed = { 5.0f, 4.0f };
    float ballRadius = 20;

    // Main game loop
    while (!WindowShouldClose()) {
        // Accumulate time for physics updates
        accumulator += GetFrameTime();

        // Update physics at a fixed time step
        while (accumulator >= fixedDeltaTime) {
            // Update ball position
            ballPosition.x += ballSpeed.x;
            ballPosition.y += ballSpeed.y;

            // Check walls collision
            if ((ballPosition.x >= (screenWidth - ballRadius)) || 
                (ballPosition.x <= ballRadius)) ballSpeed.x *= -1.0f;
            if ((ballPosition.y >= (screenHeight - ballRadius)) || 
                (ballPosition.y <= ballRadius)) ballSpeed.y *= -1.0f;

            accumulator -= fixedDeltaTime;
        }

        // Draw
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawCircleV(ballPosition, ballRadius, MAROON);
            DrawFPS(10, 10);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
