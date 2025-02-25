#pragma once
#include "Constants.h"
#include "raylib.h"

class Food {
public:
  Food(Vector2 pos);
  void Draw() const;
  bool IsConsumed() const { return consumed; }
  Vector2 GetPosition() const { return position; }
  void Consume() { consumed = true; }

public:
  static constexpr float SIZE = 6.0f; // Make food slightly bigger

private:
  Vector2 position;
  bool consumed;
};
