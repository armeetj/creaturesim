#include "Food.h"

Food::Food(Vector2 pos) 
    : position(pos)
    , consumed(false) {
}

void Food::Draw() const {
    if (!consumed) {
        // Draw food with glow effect
        DrawCircle(position.x, position.y, SIZE + 2, ColorAlpha(YELLOW, 0.2f));
        DrawCircle(position.x, position.y, SIZE, ColorAlpha(YELLOW, 0.8f));
    }
}
