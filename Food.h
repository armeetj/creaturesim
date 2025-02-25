#pragma once
#include "raylib.h"

class Food {
public:
    Food(Vector2 pos);
    void Draw() const;
    bool IsConsumed() const { return consumed; }
    Vector2 GetPosition() const { return position; }
    void Consume() { consumed = true; }

private:
    Vector2 position;
    bool consumed;
    static constexpr float SIZE = 4.0f;
};
