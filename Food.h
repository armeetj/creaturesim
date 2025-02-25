#pragma once
#include "raylib.h"

class Food {
public:
    Food(Vector2 pos);
    void Draw() const;
    bool IsConsumed() const { return consumed; }
    Vector2 GetPosition() const { return position; }
    void Consume() { consumed = true; }

public:
    static constexpr float SIZE = Constants::INITIAL_CREATURE_SIZE * 0.6f; // Make food slightly bigger

private:
    Vector2 position;
    bool consumed;
};
